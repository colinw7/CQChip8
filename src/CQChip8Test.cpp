#include <CQChip8Test.h>
#include <CQChip8.h>
#include <CChip8.h>

#include <QApplication>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>

int
main(int argc, char **argv)
{
  QApplication app(argc, argv);

  QString filename;
  bool    disassemble = false;
  bool    super       = false;

  for (int i = 1; i < argc; ++i) {
    if (argv[i][0] == '-') {
      if      (argv[i][1] == 'd')
        disassemble = true;
      else if (argv[i][1] == 's')
        super = true;
    }
    else {
      filename = argv[i];
    }
  }

  CQChip8Test *test = new CQChip8Test;

  if (filename != "")
    test->load(filename.toStdString().c_str());

  if (super)
    test->chip()->setSuper(true);

  if (disassemble)
    test->chip()->disassemble();

  test->show();

  app.exec();
}

CQChip8Test::
CQChip8Test()
{
  auto layout = new QHBoxLayout(this);
  layout->setMargin(2); layout->setSpacing(2);

  chip_ = new CQChip8;

  connect(chip_, SIGNAL(tick()), this, SLOT(updateSlot()));
  connect(chip_, SIGNAL(keyChanged()), this, SLOT(updateSlot()));

  layout->addWidget(chip_);

  //---

  auto controlFrame = new QFrame;
  auto controlLayout = new QVBoxLayout(controlFrame);
  controlLayout->setMargin(2); controlLayout->setSpacing(2);

  layout->addWidget(controlFrame);

  auto createEdit = [&](QVBoxLayout *layout, const QString &name) {
    auto editFrame  = new QFrame;
    auto editLayout = new QHBoxLayout(editFrame);
    editLayout->setMargin(2); editLayout->setSpacing(2);

    QLineEdit *edit = new QLineEdit;

    editLayout->addWidget(new QLabel(name));
    editLayout->addWidget(edit);

    layout->addWidget(editFrame);

    return edit;
  };

  pcEdit_ = createEdit(controlLayout, "PC");
  spEdit_ = createEdit(controlLayout, "SP");

  for (int i = 0; i < 16; ++i) {
    std::string str = CChip8::charStr(i);

    vEdit_[i] = createEdit(controlLayout, QString("V%1").arg(str.c_str()));
  }

  dtEdit_ = createEdit(controlLayout, "DT");
  stEdit_ = createEdit(controlLayout, "ST");

  instEdit_ = createEdit(controlLayout, "Inst");

  keysEdit_ = createEdit(controlLayout, "Keys");

  //---

  auto buttonFrame = new QFrame;
  auto buttonLayout = new QHBoxLayout(buttonFrame);
  buttonLayout->setMargin(2); buttonLayout->setSpacing(2);

  controlLayout->addWidget(buttonFrame);

  auto stepButton = new QPushButton("Step");
  auto runButton  = new QPushButton("Run");
  auto stopButton = new QPushButton("Stop");
  auto contButton = new QPushButton("Continue");

  connect(stepButton, SIGNAL(clicked()), this, SLOT(stepSlot()));
  connect(runButton , SIGNAL(clicked()), this, SLOT(runSlot()));
  connect(stopButton, SIGNAL(clicked()), this, SLOT(stopSlot()));
  connect(contButton, SIGNAL(clicked()), this, SLOT(contSlot()));

  buttonLayout->addWidget(stepButton);
  buttonLayout->addWidget(runButton);
  buttonLayout->addWidget(stopButton);
  buttonLayout->addWidget(contButton);
  buttonLayout->addStretch(1);

  //---

  controlLayout->addStretch(1);

  //---

  updateSlot();
}

void
CQChip8Test::
load(const QString &filename)
{
  chip()->load(filename.toStdString().c_str());

  updateSlot();
}

void
CQChip8Test::
setSuper(bool b)
{
  chip_->setSuper(b);
}

void
CQChip8Test::
stepSlot()
{
  chip_->step();

  updateSlot();
}

void
CQChip8Test::
runSlot()
{
  chip_->run();

  updateSlot();
}

void
CQChip8Test::
stopSlot()
{
  chip_->stop();

  updateSlot();
}

void
CQChip8Test::
contSlot()
{
  chip_->cont();

  updateSlot();
}

void
CQChip8Test::
updateSlot()
{
  auto charStr = [](uchar s) {
    return QString(CChip8::charStr(s).c_str());
  };

  auto shortStr = [](ushort s) {
    return QString(CChip8::shortStr(s).c_str());
  };

  pcEdit_->setText(shortStr(chip_->chip8()->PC()));
  spEdit_->setText(shortStr(chip_->chip8()->SP()));

  for (int i = 0; i < 16; ++i)
    vEdit_[i]->setText(charStr(chip_->chip8()->V(i)));

  dtEdit_->setText(shortStr(chip_->chip8()->DT()));
  stEdit_->setText(shortStr(chip_->chip8()->ST()));

  std::stringstream ss; chip_->chip8()->disassemble(ss, /*showAddr*/false);
  QString instStr = ss.str().c_str();

  instEdit_->setText(QString("%1 [%2 %3]").arg(instStr).
    arg(charStr(chip_->chip8()->memory(chip_->chip8()->PC()    ))).
    arg(charStr(chip_->chip8()->memory(chip_->chip8()->PC() + 1))));

  QString keysStr;

  for (int i = 0; i < 16; ++i)
    if (chip_->chip8()->isKey(i))
      keysStr += charStr(i);

  keysEdit_->setText(keysStr);
}

QSize
CQChip8Test::
sizeHint() const
{
  return QSize(800, 500);
}
