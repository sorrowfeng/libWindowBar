// Copyright (C) 2023-2024 Stdware Collections (https://www.github.com/stdware)
// Copyright (C) 2021-2023 wangwenx190 (Yuhang Zhao)
// SPDX-License-Identifier: Apache-2.0

#ifndef WINDOWBAR_H
#define WINDOWBAR_H

#include <QFrame>
#include <QAbstractButton>
#include <QMenuBar>
#include <QLabel>

// 资源初始化函数由 rcc 生成在全局命名空间(qrc_resource.cpp)。
// 若在 namespace QWK 内直接展开 Q_INIT_RESOURCE,宏内的 extern 声明会把
// 函数声明成 QWK::qInitResources_resource,而定义始终是全局的 ——
// GCC/ld 在 Linux 上会因此报 undefined reference(libWindowBar 是静态库,
// 被链进 ADS/SARibbon 共享库时必须能找到定义)。
extern int qInitResources_resource();

namespace QWK {

    class WindowBarPrivate;

    enum Theme {
      Dark,
      Light,
    };

    class WindowBar : public QFrame {
      Q_OBJECT
      Q_DECLARE_PRIVATE(WindowBar)
     public:
      explicit WindowBar(QWidget* parent = nullptr);
      ~WindowBar();

      static void initResource() {
        ::qInitResources_resource();  // 初始化资源(定义在全局命名空间)
      }

     public:
      QMenuBar* menuBar() const;
      QLabel* titleLabel() const;
      QAbstractButton* iconButton() const;
      QAbstractButton* pinButton() const;
      QAbstractButton* minButton() const;
      QAbstractButton* maxButton() const;
      QAbstractButton* closeButton() const;

      void setMenuBar(QMenuBar* menuBar);
      void setTitleLabel(QLabel* label);
      void setIconButton(QAbstractButton* btn);
      void setPinButton(QAbstractButton* btn);
      void setMinButton(QAbstractButton* btn);
      void setMaxButton(QAbstractButton* btn);
      void setCloseButton(QAbstractButton* btn);

      QMenuBar* takeMenuBar();
      QLabel* takeTitleLabel();
      QAbstractButton* takeIconButton();
      QAbstractButton* takePinButton();
      QAbstractButton* takeMinButton();
      QAbstractButton* takeMaxButton();
      QAbstractButton* takeCloseButton();

      QWidget* hostWidget() const;
      void setHostWidget(QWidget* w);

      bool titleFollowWindow() const;
      void setTitleFollowWindow(bool value);

      bool iconFollowWindow() const;
      void setIconFollowWindow(bool value);

      Theme currentTheme() const;
      void setCurrentTheme(Theme mode);

     Q_SIGNALS:
      void pinRequested(bool pin = false);
      void minimizeRequested();
      void maximizeRequested(bool max = false);
      void closeRequested();

     protected:
      bool eventFilter(QObject* obj, QEvent* event) override;

      void mousePressEvent(QMouseEvent* event) override;
      void mouseDoubleClickEvent(QMouseEvent* event) override;

      virtual void titleChanged(const QString& text);
      virtual void iconChanged(const QIcon& icon);

      void loadStyleSheet();

     protected:
      WindowBar(WindowBarPrivate& d, QWidget* parent = nullptr);

      QScopedPointer<WindowBarPrivate> d_ptr;
    };
}

#endif // WINDOWBAR_H
