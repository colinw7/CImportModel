#ifndef CQCamera3DUI_H
#define CQCamera3DUI_H

#include <CQUtil.h>

#include <QGroupBox>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>

#include <iostream>

class CQCamera3DUI {
 public:
  CQCamera3DUI(QWidget *parent, QBoxLayout *layout) :
   parent_(parent), currentLayout_(layout) {
  }

  //---

  void startLayout(QBoxLayout *layout) {
    layoutStack_.push_back(currentLayout_);

    currentLayout_ = layout;
  }

  void endLayout() {
    currentLayout_ = layoutStack_.back();

    layoutStack_.pop_back();
  }

  //---

  template<typename WIDGET>
  WIDGET *addLabelEdit(const QString &label, WIDGET *w) {
    auto *frame = new QFrame;

    auto *layout1 = new QHBoxLayout(frame);
    layout1->setMargin(2); layout1->setSpacing(2);

    layout1->addWidget(new QLabel(label));
    layout1->addWidget(w);

    currentLayout_->addWidget(frame);

    return w;
  }

  //---

  QCheckBox *addCheck(const QString &label) {
    auto *check = new QCheckBox(label);

    currentLayout_->addWidget(check);

    return check;
  }

  QPushButton *addButton(const QString &name, const char *slotName) {
    auto *button = new QPushButton(name);

    QObject::connect(button, SIGNAL(clicked()), parent_, slotName);

    currentLayout_->addWidget(button);

    return button;
  }

  void addStretch() {
    currentLayout_->addStretch(1);
  }

  //---

  void startGroup(const QString &name, bool horizontal=false) {
    groupStack_.push_back(currentGroup_);

    currentGroup_ = new QGroupBox(name);

    currentGroup_->setObjectName(name);

    QBoxLayout *layout = nullptr;

    if (horizontal)
      layout = new QHBoxLayout(currentGroup_);
    else
      layout = new QVBoxLayout(currentGroup_);

    layout->setMargin(2); layout->setSpacing(2);

    currentLayout_->addWidget(currentGroup_);

    startLayout(layout);
  }

  void endGroup() {
    endLayout();

    currentGroup_ = groupStack_.back();

    groupStack_.pop_back();
  }

  //---

  void startFrame(bool horizontal=false) {
    frameStack_.push_back(currentFrame_);

    currentFrame_ = new QFrame;

    currentFrame_->setObjectName("frame");

    QBoxLayout *layout = nullptr;

    if (horizontal)
      layout = new QHBoxLayout(currentFrame_);
    else
      layout = new QVBoxLayout(currentFrame_);

    layout->setMargin(2); layout->setSpacing(2);

    currentLayout_->addWidget(currentFrame_);

    startLayout(layout);
  }

  void endFrame() {
    endLayout();

    currentFrame_ = frameStack_.back();

    frameStack_.pop_back();
  }

  //---

  QTabWidget *startTab(const QString &name) {
    tabStack_.push_back(currentTab_);

    currentTab_ = CQUtil::makeWidget<QTabWidget>(name + "_tab");

    currentTab_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    currentLayout_->addWidget(currentTab_);

    return currentTab_;
  }

  void endTab() {
    currentTab_ = tabStack_.back();

    tabStack_.pop_back();
  }

  //---

  void startTabPage(const QString &name) {
    pageStack_.push_back(currentPage_);

    currentPage_ = CQUtil::makeWidget<QFrame>(name + "_page");

    auto *layout = new QVBoxLayout(currentPage_);

    layout->setMargin(2); layout->setSpacing(2);

    currentTab_->addTab(currentPage_, name);

    startLayout(layout);
  }

  void endTabPage() {
    endLayout();

    currentPage_ = pageStack_.back();

    pageStack_.pop_back();
  }

  //---

  void addWidget(QWidget *w) { currentLayout_->addWidget(w); }

  //---

  void validate() {
    if (! layoutStack_.empty()) std::cerr << "Bad layout stack\n";
    if (! groupStack_ .empty()) std::cerr << "Bad group stack\n";
    if (! frameStack_ .empty()) std::cerr << "Bad frame stack\n";
    if (! tabStack_   .empty()) std::cerr << "Bad tab stack\n";
    if (! pageStack_  .empty()) std::cerr << "Bad page stack\n";
  }

 private:
  QWidget *parent_ { nullptr };

  QBoxLayout*               currentLayout_ { nullptr };
  std::vector<QBoxLayout *> layoutStack_;

  QGroupBox*               currentGroup_ = nullptr;
  std::vector<QGroupBox *> groupStack_;

  QFrame*               currentFrame_ = nullptr;
  std::vector<QFrame *> frameStack_;


  QTabWidget*               currentTab_ = nullptr;
  std::vector<QTabWidget *> tabStack_;

  QFrame*               currentPage_ = nullptr;
  std::vector<QFrame *> pageStack_;
};

#endif
