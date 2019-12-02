#ifndef CQChip8_H
#define CQChip8_H

#include <QFrame>

class CChip8;

class QTimer;
class QImage;

class CQChip8 : public QFrame {
  Q_OBJECT

 public:
  CQChip8();
 ~CQChip8();

  CChip8 *chip8() const { return chip8_; }

  bool load(const QString &filename);

  void disassemble();

  void setSuper(bool b);

  void step();
  void run();
  void stop();
  void cont();

  void paintEvent(QPaintEvent *) override;

  void keyPressEvent(QKeyEvent *ke) override;
  void keyReleaseEvent(QKeyEvent *ke) override;

  QSize sizeHint() const override;

 signals:
  void tick();
  void keyChanged();

 private:
  void drawScreen();

 private slots:
  void timerSlot();

 private:
  CChip8* chip8_   { nullptr };
  int     scale_   { 8 };
  bool    running_ { false };
  QTimer* timer_   { nullptr };
  int     t_       { 0 };
  QImage* image_   { nullptr };
};

#endif
