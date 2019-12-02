#ifndef CQChip8Test_H
#define CQChip8Test_H

#include <QFrame>

class CQChip8;
class QLineEdit;

class CQChip8Test : public QFrame {
  Q_OBJECT

 public:
  CQChip8Test();

  CQChip8 *chip() const { return chip_; }

  void load(const QString &filename);

  void setSuper(bool b);

  QSize sizeHint() const override;

 private slots:
  void stepSlot();
  void runSlot();
  void stopSlot();
  void contSlot();

  void updateSlot();

 private:
  CQChip8*   chip_      { nullptr };
  QLineEdit* pcEdit_    { nullptr };
  QLineEdit* spEdit_    { nullptr };
  QLineEdit* vEdit_[16] { };
  QLineEdit* dtEdit_    { nullptr };
  QLineEdit* stEdit_    { nullptr };
  QLineEdit* instEdit_  { nullptr };
  QLineEdit* keysEdit_  { nullptr };
};

#endif
