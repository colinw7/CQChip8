#include <CQChip8.h>
#include <CChip8.h>

#include <QTimer>
#include <QImage>
#include <QPainter>
#include <QKeyEvent>

CQChip8::
CQChip8()
{
  timer_ = new QTimer(this);

  connect(timer_, SIGNAL(timeout()), this, SLOT(timerSlot()));

  timer_->start(2); // 500 Hz

  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  setFocusPolicy(Qt::StrongFocus);

  chip8_ = new CChip8;

  chip8_->reset();
}

CQChip8::
~CQChip8()
{
  delete image_;
}

bool
CQChip8::
load(const QString &filename)
{
  FILE *fp = fopen(filename.toStdString().c_str(), "r");
  if (! fp) return false;

  int i = CChip8::MemDataStart;

  uchar memory[CChip8::MemSize];

  memset(memory, 0, CChip8::MemSize);

  int c;

  while ((c = fgetc(fp)) != EOF) {
    memory[i++] = c;
  }

  chip8_->setMemory(memory);

  return true;
}

void
CQChip8::
setSuper(bool b)
{
  chip8_->setSuper(b);

  delete image_;

  image_ = nullptr;

  drawScreen();

  update();
}

void
CQChip8::
disassemble()
{
  int i = CChip8::MemDataStart;

  for (i = CChip8::MemDataStart; i <= CChip8::MemDataEnd; i += 2) {
    chip8_->disassemble(i, std::cerr);
  }
}

void
CQChip8::
timerSlot()
{
  if (running_) {
    step();
  }
  else {
    //drawScreen();

    //update();
  }
}

void
CQChip8::
run()
{
  chip8_->reset(/*memory*/false);

  cont();
}

void
CQChip8::
cont()
{
  running_ = true;

  drawScreen();

  update();
}

void
CQChip8::
step()
{
  ++t_;

  if (! chip8_->step())
    running_ = false;

  if (t_ >= 9) {
    chip8_->tick();

    drawScreen();

    update();

    t_ = 0;
  }

  if (running_) {
    if (t_ == 0) {
      emit tick();
    }
  }
}

void
CQChip8::
stop()
{
  running_ = false;

  drawScreen();

  update();
}

void
CQChip8::
drawScreen()
{
  int iw = chip8_->screenWidth ();
  int ih = chip8_->screenHeight();

  int siw = iw*scale_;
  int sih = ih*scale_;

  if (! image_ || image_->width() != siw || image_->height() != sih) {
    image_ = new QImage(siw, sih, QImage::Format_ARGB32_Premultiplied);
  }

  QPainter painter(image_);

  painter.fillRect(QRect(0, 0, siw, sih), QColor(0, 0, 0));

  int is = 0;
  int iy = 0;

  for (int y = 0; y < ih; ++y) {
    int ix = 0;

    for (int x = 0; x < iw; ++x, ++is) {
      if (chip8_->screen(is))
        painter.fillRect(QRect(ix, iy, scale_, scale_), QColor(255, 255, 255));

      ix += scale_;
    }

    iy += scale_;
  }
}

void
CQChip8::
paintEvent(QPaintEvent *)
{
  QPainter p(this);

  p.fillRect(rect(), QColor(50, 50, 50));

  if (image_)
    p.drawImage(0, 0, *image_);
}

void
CQChip8::
keyPressEvent(QKeyEvent *ke)
{
  // 1 2 3 C   1 2 3 4
  // 4 5 6 D   Q W E R
  // 7 8 9 E   A S D F
  // A 0 B F   Z X C V
  int k = ke->key();

  if      (k == Qt::Key_1) chip8_->setKey(0x1, true);
  else if (k == Qt::Key_2) chip8_->setKey(0x2, true);
  else if (k == Qt::Key_3) chip8_->setKey(0x3, true);
  else if (k == Qt::Key_4) chip8_->setKey(0xC, true);
  else if (k == Qt::Key_Q) chip8_->setKey(0x4, true);
  else if (k == Qt::Key_W) chip8_->setKey(0x5, true);
  else if (k == Qt::Key_E) chip8_->setKey(0x6, true);
  else if (k == Qt::Key_R) chip8_->setKey(0xD, true);
  else if (k == Qt::Key_A) chip8_->setKey(0x7, true);
  else if (k == Qt::Key_S) chip8_->setKey(0x8, true);
  else if (k == Qt::Key_D) chip8_->setKey(0x9, true);
  else if (k == Qt::Key_F) chip8_->setKey(0xE, true);
  else if (k == Qt::Key_Z) chip8_->setKey(0xA, true);
  else if (k == Qt::Key_X) chip8_->setKey(0x0, true);
  else if (k == Qt::Key_C) chip8_->setKey(0xB, true);
  else if (k == Qt::Key_V) chip8_->setKey(0xF, true);
  else return;

  emit keyChanged();
}

void
CQChip8::
keyReleaseEvent(QKeyEvent *ke)
{
  // 1 2 3 4
  // Q W E R
  // A S D F
  // Z X C V
  int k = ke->key();

  if      (k == Qt::Key_1) chip8_->setKey(0x1, false);
  else if (k == Qt::Key_2) chip8_->setKey(0x2, false);
  else if (k == Qt::Key_3) chip8_->setKey(0x3, false);
  else if (k == Qt::Key_4) chip8_->setKey(0xC, false);
  else if (k == Qt::Key_Q) chip8_->setKey(0x4, false);
  else if (k == Qt::Key_W) chip8_->setKey(0x5, false);
  else if (k == Qt::Key_E) chip8_->setKey(0x6, false);
  else if (k == Qt::Key_R) chip8_->setKey(0xD, false);
  else if (k == Qt::Key_A) chip8_->setKey(0x7, false);
  else if (k == Qt::Key_S) chip8_->setKey(0x8, false);
  else if (k == Qt::Key_D) chip8_->setKey(0x9, false);
  else if (k == Qt::Key_F) chip8_->setKey(0xE, false);
  else if (k == Qt::Key_Z) chip8_->setKey(0xA, false);
  else if (k == Qt::Key_X) chip8_->setKey(0x0, false);
  else if (k == Qt::Key_C) chip8_->setKey(0xB, false);
  else if (k == Qt::Key_V) chip8_->setKey(0xF, false);
  else return;

  emit keyChanged();
}

QSize
CQChip8::
sizeHint() const
{
  int iw = chip8_->screenWidth ();
  int ih = chip8_->screenHeight();

  return QSize(iw*scale_, ih*scale_);
}
