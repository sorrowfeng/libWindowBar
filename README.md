# libWindowBar
搭配QWindowKit使用的windowbar组件, 从QWindowKit中单独提取出来

# 示例图

![250628101627049.png](https://fastly.jsdelivr.net/gh/sorrowfeng/ImageHub@main/2025-06/250628101627049_1751076987077.png)

# 使用说明

cmake导入
```cmake
add_subdirectory(libWindowBar/libWindowBar)
target_link_libraries(${PROJECT_NAME} PRIVATE libWindowBar)
```

main.cpp
```cpp

int main(int argc, char* argv[]) {

#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
  QGuiApplication::setHighDpiScaleFactorRoundingPolicy(
      Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif

  QCoreApplication::setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);

  QApplication a(argc, argv);
  MainWindow w;
  w.show();

  return a.exec();
}

```

mainwindow.h
```cpp
#include <QMainWindow>

namespace QWK {
class WidgetWindowAgent;
class StyleAgent;
}  // namespace QWK

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget* parent = nullptr);
  ~MainWindow();

 protected:
  bool event(QEvent* event) override;

 private:
  void installWindowAgent();

  QWK::WidgetWindowAgent* windowAgent;
};

```

mainwindow.cpp
```cpp

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QPainter>
#include <QPushButton>
#include <QStyle>
#include <QTime>
#include <QTimer>
#include <QWindow>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QtGui/QActionGroup>
#else
#include <QtWidgets/QActionGroup>
#endif

#include <QWKWidgets/widgetwindowagent.h>

#include "libWindowBar/windowbar.h"
#include "libWindowBar/windowbutton.h"



MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
  setAttribute(Qt::WA_DontCreateNativeAncestors);
  installWindowAgent();
}


static inline void emulateLeaveEvent(QWidget* widget) {
  Q_ASSERT(widget);
  if (!widget) {
    return;
  }
  QTimer::singleShot(0, widget, [widget]() {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    const QScreen* screen = widget->screen();
#else
    const QScreen *screen = widget->windowHandle()->screen();
#endif
    const QPoint globalPos = QCursor::pos(screen);
    if (!QRect(widget->mapToGlobal(QPoint{0, 0}), widget->size())
             .contains(globalPos)) {
      QCoreApplication::postEvent(widget, new QEvent(QEvent::Leave));
      if (widget->testAttribute(Qt::WA_Hover)) {
        const QPoint localPos = widget->mapFromGlobal(globalPos);
        const QPoint scenePos = widget->window()->mapFromGlobal(globalPos);
        static constexpr const auto oldPos = QPoint{};
        const Qt::KeyboardModifiers modifiers =
            QGuiApplication::keyboardModifiers();
#if (QT_VERSION >= QT_VERSION_CHECK(6, 4, 0))
        const auto event = new QHoverEvent(QEvent::HoverLeave, scenePos,
                                           globalPos, oldPos, modifiers);
        Q_UNUSED(localPos);
#elif (QT_VERSION >= QT_VERSION_CHECK(6, 3, 0))
        const auto event =  new QHoverEvent(QEvent::HoverLeave, localPos, globalPos, oldPos, modifiers);
        Q_UNUSED(scenePos);
#else
        const auto event =  new QHoverEvent(QEvent::HoverLeave, localPos, oldPos, modifiers);
        Q_UNUSED(scenePos);
#endif
        QCoreApplication::postEvent(widget, event);
      }
    }
  });
}

bool MainWindow::event(QEvent* event) {
  switch (event->type()) {
    case QEvent::WindowActivate: {
      auto menu = menuWidget();
      if (menu) {
        menu->setProperty("bar-active", true);
        style()->polish(menu);
      }
      break;
    }

    case QEvent::WindowDeactivate: {
      auto menu = menuWidget();
      if (menu) {
        menu->setProperty("bar-active", false);
        style()->polish(menu);
      }
      break;
    }

    default:
      break;
  }
  return QMainWindow::event(event);
}

void MainWindow::installWindowAgent() {
  windowAgent = new QWK::WidgetWindowAgent(this);
  windowAgent->setup(this);

  auto titleLabel = new QLabel();
  titleLabel->setAlignment(Qt::AlignCenter);
  titleLabel->setObjectName(QStringLiteral("win-title-label"));

#ifndef Q_OS_MAC
  auto iconButton = new QWK::WindowButton();
  iconButton->setObjectName(QStringLiteral("icon-button"));
  iconButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

  auto pinButton = new QWK::WindowButton();
  pinButton->setCheckable(true);
  pinButton->setObjectName(QStringLiteral("pin-button"));
  pinButton->setProperty("system-button", true);
  pinButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

  auto minButton = new QWK::WindowButton();
  minButton->setObjectName(QStringLiteral("min-button"));
  minButton->setProperty("system-button", true);
  minButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

  auto maxButton = new QWK::WindowButton();
  maxButton->setCheckable(true);
  maxButton->setObjectName(QStringLiteral("max-button"));
  maxButton->setProperty("system-button", true);
  maxButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

  auto closeButton = new QWK::WindowButton();
  closeButton->setObjectName(QStringLiteral("close-button"));
  closeButton->setProperty("system-button", true);
  closeButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
#endif

  QWK::WindowBar::initResource();

  auto windowBar = new QWK::WindowBar();
#ifndef Q_OS_MAC
  windowBar->setIconButton(iconButton);
  windowBar->setPinButton(pinButton);
  windowBar->setMinButton(minButton);
  windowBar->setMaxButton(maxButton);
  windowBar->setCloseButton(closeButton);
#endif
  windowBar->setTitleLabel(titleLabel);
  windowBar->setHostWidget(this);

  windowAgent->setTitleBar(windowBar);
#ifndef Q_OS_MAC
  windowAgent->setHitTestVisible(pinButton, true);
  windowAgent->setSystemButton(QWK::WindowAgentBase::WindowIcon, iconButton);
  windowAgent->setSystemButton(QWK::WindowAgentBase::Minimize, minButton);
  windowAgent->setSystemButton(QWK::WindowAgentBase::Maximize, maxButton);
  windowAgent->setSystemButton(QWK::WindowAgentBase::Close, closeButton);
#endif

#ifdef Q_OS_MAC
  windowAgent->setSystemButtonAreaCallback([](const QSize& size) {
    static constexpr const int width = 75;
    return QRect(QPoint(size.width() - width, 0),
                 QSize(width, size.height()));  //
  });
#endif

  setMenuWidget(windowBar);

#ifndef Q_OS_MAC
  connect(
      windowBar, &QWK::WindowBar::pinRequested, this,
      [this, pinButton](bool pin) {
        if (isHidden() || isMinimized() || isMaximized() || isFullScreen()) {
          return;
        }
        setWindowFlag(Qt::WindowStaysOnTopHint, pin);
        show();
        pinButton->setChecked(pin);
      });
  connect(windowBar, &QWK::WindowBar::minimizeRequested, this,
          &QWidget::showMinimized);
  connect(
      windowBar, &QWK::WindowBar::maximizeRequested, this,
      [this, maxButton](bool max) {
        if (max) {
          showMaximized();
        } else {
          showNormal();
        }

        // It's a Qt issue that if a QAbstractButton::clicked triggers a window's maximization,
        // the button remains to be hovered until the mouse move. As a result, we need to
        // manually send leave events to the button.
        emulateLeaveEvent(maxButton);
      });
  connect(windowBar, &QWK::WindowBar::closeRequested, this, &QWidget::close);
#endif
}

```