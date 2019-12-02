#ifndef CChip8_H
#define CChip8_H

#include <random>
#include <iostream>
#include <sstream>
#include <cassert>

class CChip8 {
 public:
  static const ushort MemStart      = 0x000;
  static const ushort MemDataStart  = 0x200;
  static const ushort MemDataStart1 = 0x600;
  static const ushort MemDataEnd    = 0xfff;

  static const int MemSize = 0x1000;

  static const int CharHeight      = 5;
  static const int SuperCharHeight = 10;

//static const ushort SpriteAddr = 0xfff - 16*CharHeight;
  static const ushort SpriteAddr = 0;

 private:
  static const uchar NumV      = 16;
  static const uchar StackSize = 16;
  static const uchar NumKeys   = 16;

  static const uchar  DisplayWidth  = 64;
  static const uchar  DisplayHeight = 32;
  static const ushort DisplaySize   = DisplayWidth*DisplayHeight;

  static const uchar  SuperDisplayWidth  = 128;
  static const uchar  SuperDisplayHeight = 64;
  static const ushort SuperDisplaySize   = SuperDisplayWidth*SuperDisplayHeight;

 public:
  CChip8() { }

  ushort PC() const { return PC_; }

  void setPC(ushort PC) {
    assert(PC >= MemDataStart && PC <= MemDataEnd);
    PC_ = PC;
  }

  void nextOp() { PC_ += 2; }

  uchar SP() const { return SP_; }
  void setSP(uchar SP) { assert(SP < StackSize); SP_ = SP; }

  ushort popSP() { assert(SP_ > 0); return stack_[--SP_]; }
  void pushSP(ushort v) { assert(SP_ < StackSize); stack_[SP_++] = v; }

  uchar V(uchar i) const { assert(i < NumV); return V_[i]; }
  void setV(uchar i, uchar v) { assert(i < NumV); V_[i] = v; }

  void setVF(uchar v) { *VF_ = v; }

  uchar R(uchar i) const { assert(i < NumV); return R_[i]; }
  void setR(uchar i, uchar v) { assert(i < NumV); R_[i] = v; }

  ushort I() const { return I_; }

  bool setI(ushort I) {
    bool f = (I > MemDataEnd);
    I_ = (I & MemDataEnd);
    return f;
  }

  uchar DT() const { return DT_; }
  void setDT(uchar c) { DT_ = c; }

  uchar ST() const { return ST_; }
  void setST(uchar c) { ST_ = c; }

  //---

  ushort screenWidth() {
    if (isSuper() && isHighRes())
      return SuperDisplayWidth;

    return DisplayWidth;
  }

  ushort screenHeight() {
    if (isSuper() && isHighRes())
      return SuperDisplayHeight;

    return DisplayHeight;
  }

  uchar *pscreen() { return (isSuper() ? superScreen_ : screen_); }

  uchar screen(int pos) { return (isSuper() ? superScreen_[pos] : screen_[pos]); }

  //---

  uchar memory(ushort pos) { assert(pos <= MemDataEnd); return memory_[pos]; }

  void setMemory(ushort pos, uchar v) {
    assert(pos >= MemDataStart && pos <= MemDataEnd);
    memory_[pos] = v;
  }

  //---

  bool isSuper() const { return superChip48_; }
  void setSuper(bool b) { superChip48_ = b; }

  bool isHighRes() const { return highRes_; }
  void setHighRes(bool b) { highRes_ = b; }

  //---

  bool isKey(uchar k) { assert(k < NumKeys); return keys_[k]; }

  void setKey(uchar k, bool b) {
    assert(k < NumKeys); keys_[k] = (b ? 1 : 0); if (b) keyPressed_ = k + 1; }

  //---

  static std::string shortStr(ushort s) { std::stringstream ss;
    ss << std::uppercase << std::hex << int(s); return ss.str(); };

  static std::string charStr(uchar c) { std::stringstream ss;
    ss << std::uppercase << std::hex << int(c); return ss.str(); };

  //---

  void reset(bool resetMemory=true) {
    if (resetMemory)
      memset(memory_, 0, MemSize*sizeof(memory_[0]));

    initDigitSprites();

    //---

    memset(V_, 0, NumV*sizeof(V_[0]));

    setI(0);

    setDT(0);
    setST(0);

    setPC(MemDataStart);

    setSP(0);

    memset(stack_, 0, StackSize*sizeof(ushort));

    memset(keys_, 0, NumKeys*sizeof(uchar));

    clearScreen();

//  memset(sprites_     , 0, 16*sizeof(Sprite));
//  memset(superSprites_, 0, 16*sizeof(SuperSprite));
  }

  //---

  void setMemory(const uchar *m) {
    memcpy(&memory_[MemDataStart], &m[MemDataStart], MemSize - MemDataStart);
  }

  //---

  void scrollDown(uchar n) {
    int sw = screenWidth ();
    int sh = screenHeight();
    int ss = sw*sh;

    uchar *screen = this->pscreen();

    //---

    int len   = n*sw;
    int start = ss - len;

    // bottom n lines to buffer
    memcpy(scrollBuffer, &screen[start], len);

    // scroll down n
    memmove(screen, &screen[len], start);

    // buffer to top n lines
    memcpy(screen, scrollBuffer, len);
  }

  void scrollLeft(uchar n) {
    int sw = screenWidth ();
    int sh = screenHeight();

    uchar *screen = this->pscreen();

    //---

    int n1 = sw - n;

    int pos = 0;

    for (int y = 0; y < sh; ++y) {
      // left n to buffer
      memcpy(scrollBuffer, &screen[pos], n);

      // scroll left (width - n)
      memmove(&screen[pos], &screen[pos + n], n1);

      // buffer to right n
      memcpy(&screen[pos + n1], scrollBuffer, n);

      pos += sw;
    }
  }

  void scrollRight(uchar n) {
    int sw = screenWidth ();
    int sh = screenHeight();

    uchar *screen = this->pscreen();

    //---

    int n1 = sw - n;

    int pos = 0;

    for (int y = 0; y < sh; ++y) {
      // right n to buffer
      memcpy(scrollBuffer, &screen[pos + n1], n);

      // scroll right (width - n)
      memmove(&screen[pos + n], &screen[pos], n1);

      // buffer to left n
      memcpy(&screen[pos], scrollBuffer, n);

      pos += sw;
    }
  }

  void quit() { }

  //---

  bool step() {
    bool rc = true;

    if (waitKey_) {
      if (keyPressed_) {
        setV(waitInd_, keyPressed_ - 1);
        keyPressed_  = 0;
        waitKey_     = false;
      }

      return rc;
    }

    //---

    uchar b0   = memory(PC()    );
    uchar byte = memory(PC() + 1);

    nextOp();

    //---

    // <op> <x> <y> <v3>
    uchar op = (b0   & 0xF0) >> 4;
    uchar x  =  b0   & 0x0F;
    uchar y  = (byte & 0xF0) >> 4;
    uchar v3 =  byte & 0x0F;

    auto addr = [&]() { return (x << 8) | byte; };

    //---

    switch (op) {
      case 0x0: {
        if      (byte == 0x00) {
          // NOP
          rc = false;
        }
        else if (byte == 0xE0) {
          clearScreen(); // CLS
        }
        else if (byte == 0xEE) {
          setPC(popSP()); // RET
        }
        else if (y == 0xC) {
          if (isSuper())
            scrollDown(v3); // SCD nibble
          else
            assert(false);
        }
        else if (byte == 0xFB) {
          if (isSuper())
            scrollRight(isHighRes() ? 4 : 2); // SCR (4 or 2 pixels)
          else
            assert(false);
        }
        else if (byte == 0xFC) {
          if (isSuper())
            scrollLeft(isHighRes() ? 4 : 2); // SCL (4 or 2 pixels)
          else
            assert(false);
        }
        else if (byte == 0xFD) {
          if (isSuper())
            quit(); // EXIT
          else
            assert(false);
        }
        else if (byte == 0xFE) {
          if (isSuper())
            setHighRes(false); // LOW
          else
            assert(false);
        }
        else if (byte == 0xFF) {
          if (isSuper())
            setHighRes(true); // HIGH (128x64)
          else
            assert(false);
        }
        else {
          setPC(addr()); // SYS addr
        }

        break;
      }
      case 0x1: { // JP addr
        setPC(addr());
        break;
      }
      case 0x2: { // CALL addr
        pushSP(PC());
        setPC(addr());

        break;
      }
      case 0x3: // SE Vx, byte
        if (V(x) == byte) nextOp();
        break;
      case 0x4: // SNE Vx, byte
        if (V(x) != byte) nextOp();
        break;
      case 0x5: // SE Vx, Vy
        if (V(x) == V(y)) nextOp();
        break;
      case 0x6: // LD Vx, byte
        setV(x, byte);
        break;
      case 0x7: // ADD Vx, byte
        setV(x, V(x) + byte);
        break;
      case 0x8: {
        // LD Vx, Vy
        if      (v3 == 0x0) setV(x, V(y));
        // OR Vx, Vy
        else if (v3 == 0x1) setV(x, V(x) | V(y));
        // AND Vx, Vy
        else if (v3 == 0x2) setV(x, V(x) & V(y));
        // XOR Vx, Vy
        else if (v3 == 0x3) setV(x, V(x) ^ V(y));
        // ADD Vx, Vy
        else if (v3 == 0x4) {
          ushort sum = V(x) + V(y); setVF(sum > 0xFF ? 1 : 0); setV(x, sum & 0xFF);
        }
        // SUB Vx, Vy
        else if (v3 == 0x5) { setVF(V(x) >= V(y) ? 1 : 0); setV(x, V(x) - V(y)); }
        // SHR Vx, Vy
        else if (v3 == 0x6) {
        //setVF(V(y) & 1 ? 1 : 0); setV(x, V(y) >> 1);
          setVF(V(x) & 1 ? 1 : 0); setV(x, V(x) >> 1); // depends on mode if y used
        }
        // SUBN Vx, Vy
        else if (v3 == 0x7) { setVF(V(y) >= V(x) ? 1 : 0); setV(x, V(y) - V(x)); }
        // SHL Vx, Vy
        else if (v3 == 0xE) {
        //setVF(V(y) & 0x80 ? 1 : 0); setV(x, V(y) << 1);
          setVF(V(x) & 0x80 ? 1 : 0); setV(x, V(x) << 1); // depends on mode if y used
        }

        else assert(false);

        break;
      }
      case 0x9: // SNE Vx, Vy
        if (V(x) != V(y)) nextOp();
        break;
      case 0xa: { // LD I, addr
        setI(addr());
        break;
      }
      case 0xb: { // JP V0, addr
        setPC(addr() + V(0));
        break;
      }
      case 0xc: // RND Vx, byte
        setV(x, rand() & byte);
        break;
      case 0xd: // DRW Vx, Vy, nibble
        if (isSuper() && v3 == 0) {
          // DRW Vx, Vy, 0 ???
        }
        setVF(drawSprite(&memory_[I()], v3, V(x), V(y)));
        break;
      case 0xe: {
        // SKP Vx
        if      (byte == 0x9E) {
          if (isKey(V(x)))
            nextOp();
        }
        // SKNP Vx
        else if (byte == 0xA1) {
          if (! isKey(V(x)))
            nextOp();
        }

        else assert(false);

        break;
      }
      case 0xf: {
        // LD Vx, DT
        if      (byte == 0x07) setV(x, DT());
        // LD Vx, K
        else if (byte == 0x0A) { keyPressed_ = 0; waitInd_ = x; waitKey_ = true; }
        // LD DT, Vx
        else if (byte == 0x15) setDT(V(x));
        // LD ST, Vx
        else if (byte == 0x18) setST(V(x));
        // ADD I, Vx
        else if (byte == 0x1e) setVF(setI(I() + V(x)));
        // LD F, Vx
        else if (byte == 0x29) setI(SpriteAddr + V(x)*CharHeight);
        // LD B, Vx
        else if (byte == 0x33) {
          // binary coded decimal
          uchar i = V(x);

          uchar d0 =  i / 100;
          uchar d1 = (i % 100)/10;
          uchar d2 =  i % 10;

          setMemory(I()    , d0);
          setMemory(I() + 1, d1);
          setMemory(I() + 2, d2);
        }
        // LD [I], Vx
        else if (byte == 0x55) {
          for (int i = 0; i <= x; ++i)
            setMemory(I() + i, V(i));

          //setI(I() + x + 1);
        }
        // LD Vx, [I]
        else if (byte == 0x65) {
          for (int i = 0; i <= x; ++i)
            setV(i, memory(I() + i));

          //setI(I() + x + 1);
        }

        else if (byte == 0x30) {
          if (isSuper()) {
            // I = HighSpriteAddr + V(x)*10; // LD HF, Vx
          }
          else
            assert(false);
        }
        else if (byte == 0x75) {
          if (isSuper()) {
            // LD R, Vx
            for (int i = 0; i <= x; ++i)
              setR(i, V(i));
          }
          else
            assert(false);
        }
        else if (byte == 0x85) {
          if (isSuper()) {
            // LD Vx, R
            for (int i = 0; i <= x; ++i)
              setV(i, R(i));
          }
          else
            assert(false);
        }

        else assert(false);

        break;
      }
      default: {
        assert(false);
        break;
      }
    }

    return rc;
  }

  //---

  void tick() {
    if (DT() > 0) setDT(DT() - 1);
    if (ST() > 0) setST(ST() - 1);
  }

  //---

  void disassemble(std::ostream &os, bool showAddr=true) {
    disassemble(PC(), os, showAddr);
  }

  void disassemble(ushort PC, std::ostream &os, bool showAddr=true) {
    if (showAddr)
      os << shortStr(PC) << " : ";

    //---

    uchar b0   = memory(PC++);
    uchar byte = memory(PC++);

    // <op> <x> <y> <v3>
    uchar op = (b0   & 0xF0) >> 4;
    uchar x  =  b0   & 0x0F;
    uchar y  = (byte & 0xF0) >> 4;
    uchar v3 =  byte & 0x0F;

    auto addr = [&]() { return (x << 8) | byte; };

    //---

    auto badOp = [&]() { os << "Bad OP " << charStr(b0) << " " << charStr(byte) << "\n"; };

    //---

    switch (op) {
      case 0x0: {
        if      (byte == 0x00) os << "NOP\n";
        else if (byte == 0xE0) os << "CLS\n";
        else if (byte == 0xEE) os << "RET\n";
        else if (y    == 0xC) {
          os << "SCD " << charStr(v3) << "\n";
        }
        else if (byte == 0xFB) {
          os << "SCR\n";
        }
        else if (byte == 0xFC) {
          os << "SCL\n";
        }
        else if (byte == 0xFD) {
          os << "EXIT\n";
        }
        else if (byte == 0xFE) {
          os << "LOW\n";
        }
        else if (byte == 0xFF) {
          os << "HIGH\n";
        }
        else {
          os << "SYS " << shortStr(addr()) << "\n";
        }

        break;
      }
      case 0x1: { // JP addr
        os << "JP " << shortStr(addr()) << "\n";
        break;
      }
      case 0x2: { // CALL addr
        os << "CALL " << shortStr(addr()) << "\n";
        break;
      }
      case 0x3: // SE Vx, byte
        os << "SE V" << charStr(x) << ", " << charStr(byte) << "\n";
        break;
      case 0x4: // SNE Vx, byte
        os << "SNE V" << charStr(x) << ", " << charStr(byte) << "\n";
        break;
      case 0x5: // SE Vx, Vy
        os << "SE V" << charStr(x) << ", V" << charStr(y) << "\n";
        break;
      case 0x6: // LD Vx, byte
        os << "LD V" << charStr(x) << ", " << charStr(byte) << "\n";
        break;
      case 0x7: // ADD Vx, byte
        os << "ADD V" << charStr(x) << ", " << charStr(byte) << "\n";
        break;
      case 0x8: {
        // LD Vx, Vy
        if      (v3 == 0x0) os << "LD V" << charStr(x) << ", V" << charStr(y) << "\n";
        // OR Vx, Vy
        else if (v3 == 0x1) os << "OR V" << charStr(x) << ", V" << charStr(y) << "\n";
        // AND Vx, Vy
        else if (v3 == 0x2) os << "AND V" << charStr(x) << ", V" << charStr(y) << "\n";
        // XOR Vx, Vy
        else if (v3 == 0x3) os << "XOR V" << charStr(x) << ", V" << charStr(y) << "\n";
        // ADD Vx, Vy
        else if (v3 == 0x4) os << "ADD V" << charStr(x) << ", V" << charStr(y) << "\n";
        // SUB Vx, Vy
        else if (v3 == 0x5) os << "SUB V" << charStr(x) << ", V" << charStr(y) << "\n";
        // SHR Vx {, Vy }
        else if (v3 == 0x6) os << "SHR V" << charStr(x) << ", V" << charStr(y) << "\n";
        // SUBN Vx, Vy
        else if (v3 == 0x7) os << "SUBN V" << charStr(x) << ", V" << charStr(y) << "\n";
        // SHL Vx {, Vy }
        else if (v3 == 0xE) os << "SHL V" << charStr(x) << ", V" << charStr(y) << "\n";

        else badOp();

        break;
      }
      case 0x9: // SNE Vx, Vy
        os << "SNE V" << charStr(x) << ", V" << charStr(y) << "\n";
        break;
      case 0xa: { // LD I, addr
        os << "LD I, " << shortStr(addr()) << "\n";
        break;
      }
      case 0xb: { // JP V0, addr
        os << "JP V0, " << shortStr(addr()) << "\n";
        break;
      }
      case 0xc: // RND Vx, byte
        os << "RND V" << charStr(x) << ", " << charStr(byte) << "\n";
        break;
      case 0xd: // DRW Vx, Vy, nibble
        if (isSuper() && v3 == 0) {
          // os << "DRW V" << charStr(x) << ", V" << charStr(y) << ", 0\n"; ???
        }
        os << "DRW V" << charStr(x) << ", V" << charStr(y) << ", " << charStr(v3) << "\n";
        break;
      case 0xe: {
        // SKP Vx
        if      (byte == 0x9E) os << "SKP V" << charStr(x) << "\n";
        // SKNP Vx
        else if (byte == 0xA1) os << "SKNP V" << charStr(x) << "\n";

        else badOp();

        break;
      }
      case 0xf: {
        // LD Vx, DT
        if      (byte == 0x07) os << "LD V" << charStr(x) << ", DT\n";
        // LD Vx, K
        else if (byte == 0x0A) os << "LD V" << charStr(x) << ", K\n";
        // LD DT, Vx
        else if (byte == 0x15) os << "LD DT, V" << charStr(x) << "\n";
        // LD ST, Vx
        else if (byte == 0x18) os << "LD ST, V" << charStr(x) << "\n";
        // ADD I, Vx
        else if (byte == 0x1e) os << "ADD I, V" << charStr(x) << "\n";
        // LD F, Vx
        else if (byte == 0x29) os << "LD F, V" << charStr(x) << "\n";
        // LD B, Vx
        else if (byte == 0x33) os << "LD B, V" << charStr(x) << "\n";
        // LD [I], Vx
        else if (byte == 0x55) os << "LD [I], V" << charStr(x) << "\n";
        // LD Vx, [I]
        else if (byte == 0x65) os << "LD V" << charStr(x) << ", [I]\n";

        else if (byte == 0x30) os << "LD HF, V" << charStr(x) << "\n";
        else if (byte == 0x75) os << "LD R, V" << charStr(x) << "\n";
        else if (byte == 0x85) os << "LD V" << charStr(x) << ", R\n";

        else badOp();

        break;
      }
      default: {
        badOp();
        break;
      }
    }
  }

 private:
  void initDigitSprites() {
    // "0"
    // ****
    // *  *
    // *  *
    // *  *
    // ****
    memory_[SpriteAddr + 0] = 0xF0;
    memory_[SpriteAddr + 1] = 0x90;
    memory_[SpriteAddr + 2] = 0x90;
    memory_[SpriteAddr + 3] = 0x90;
    memory_[SpriteAddr + 4] = 0xF0;

    // "1"
    //   *
    //  **
    //   *
    //   *
    //  ***
    memory_[SpriteAddr + 5] = 0x20;
    memory_[SpriteAddr + 6] = 0x60;
    memory_[SpriteAddr + 7] = 0x20;
    memory_[SpriteAddr + 8] = 0x20;
    memory_[SpriteAddr + 9] = 0x70;

    // "2"
    // ****
    //    *
    // ****
    // *
    // ****
    memory_[SpriteAddr + 10] = 0xF0;
    memory_[SpriteAddr + 11] = 0x10;
    memory_[SpriteAddr + 12] = 0xF0;
    memory_[SpriteAddr + 13] = 0x80;
    memory_[SpriteAddr + 14] = 0xF0;

    // "3"
    // ****
    //    *
    // ****
    //    *
    // ****
    memory_[SpriteAddr + 15] = 0xF0;
    memory_[SpriteAddr + 16] = 0x10;
    memory_[SpriteAddr + 17] = 0xF0;
    memory_[SpriteAddr + 18] = 0x10;
    memory_[SpriteAddr + 19] = 0xF0;

    // "4"
    // *  *
    // *  *
    // ****
    //    *
    //    *
    memory_[SpriteAddr + 20] = 0x90;
    memory_[SpriteAddr + 21] = 0x90;
    memory_[SpriteAddr + 22] = 0xF0;
    memory_[SpriteAddr + 23] = 0x10;
    memory_[SpriteAddr + 24] = 0x10;

    // "5"
    // ****
    // *
    // ****
    //    *
    // ****
    memory_[SpriteAddr + 25] = 0xF0;
    memory_[SpriteAddr + 26] = 0x80;
    memory_[SpriteAddr + 27] = 0xF0;
    memory_[SpriteAddr + 28] = 0x10;
    memory_[SpriteAddr + 29] = 0xF0;

    // "6"
    // ****
    // *
    // ****
    // *  *
    // ****
    memory_[SpriteAddr + 30] = 0xF0;
    memory_[SpriteAddr + 31] = 0x80;
    memory_[SpriteAddr + 32] = 0xF0;
    memory_[SpriteAddr + 33] = 0x90;
    memory_[SpriteAddr + 34] = 0xF0;

    // "7"
    // ****
    //    *
    //   *
    //  *
    //  *
    memory_[SpriteAddr + 35] = 0xF0;
    memory_[SpriteAddr + 36] = 0x10;
    memory_[SpriteAddr + 37] = 0x20;
    memory_[SpriteAddr + 38] = 0x40;
    memory_[SpriteAddr + 39] = 0x40;

    // "8"
    // ****
    // *  *
    // ****
    // *  *
    // ****
    memory_[SpriteAddr + 40] = 0xF0;
    memory_[SpriteAddr + 41] = 0x90;
    memory_[SpriteAddr + 42] = 0xF0;
    memory_[SpriteAddr + 43] = 0x90;
    memory_[SpriteAddr + 44] = 0xF0;

    // "9"
    // ****
    // *  *
    // ****
    //    *
    // ****
    memory_[SpriteAddr + 45] = 0xF0;
    memory_[SpriteAddr + 46] = 0x90;
    memory_[SpriteAddr + 47] = 0xF0;
    memory_[SpriteAddr + 48] = 0x10;
    memory_[SpriteAddr + 49] = 0xF0;

    // "A"
    // ****
    // *  *
    // ****
    // *  *
    // *  *
    memory_[SpriteAddr + 50] = 0xF0;
    memory_[SpriteAddr + 51] = 0x90;
    memory_[SpriteAddr + 52] = 0xF0;
    memory_[SpriteAddr + 53] = 0x90;
    memory_[SpriteAddr + 54] = 0x90;

    // "B"
    // ***
    // *  *
    // ***
    // *  *
    // ***
    memory_[SpriteAddr + 55] = 0xE0;
    memory_[SpriteAddr + 56] = 0x90;
    memory_[SpriteAddr + 57] = 0xE0;
    memory_[SpriteAddr + 58] = 0x90;
    memory_[SpriteAddr + 59] = 0xE0;

    // "C"
    // ****
    // *
    // *
    // *
    // ****
    memory_[SpriteAddr + 60] = 0xF0;
    memory_[SpriteAddr + 61] = 0x80;
    memory_[SpriteAddr + 62] = 0x80;
    memory_[SpriteAddr + 63] = 0x80;
    memory_[SpriteAddr + 64] = 0xF0;

    // "D"
    // ***
    // *  *
    // *  *
    // *  *
    // ***
    memory_[SpriteAddr + 65] = 0xE0;
    memory_[SpriteAddr + 66] = 0x90;
    memory_[SpriteAddr + 67] = 0x90;
    memory_[SpriteAddr + 68] = 0x90;
    memory_[SpriteAddr + 69] = 0xE0;

    // "E"
    // ****
    // *
    // ****
    // *
    // ****
    memory_[SpriteAddr + 70] = 0xF0;
    memory_[SpriteAddr + 71] = 0x80;
    memory_[SpriteAddr + 72] = 0xF0;
    memory_[SpriteAddr + 73] = 0x80;
    memory_[SpriteAddr + 74] = 0xF0;

    // "F"
    // ****
    // *
    // ****
    // *
    // *
    memory_[SpriteAddr + 75] = 0xF0;
    memory_[SpriteAddr + 76] = 0x80;
    memory_[SpriteAddr + 77] = 0xF0;
    memory_[SpriteAddr + 78] = 0x80;
    memory_[SpriteAddr + 79] = 0x80;
  }

  //---

  uchar drawSprite(const uchar *addr, uchar len, uchar x, uchar y) {
    uchar hit = 0;

    int sw = screenWidth ();
    int sh = screenHeight();
    int ss = sw*sh;

    uchar *screen = this->pscreen();

    ushort pos = y*sw + x;

    for (int i = 0; i < len; ++i) {
      while (pos >= ss)
        pos -= ss;

      uchar pixels = addr[i];

      for (int px = 0; px < 8; ++px) {
        int px1 = 7 - px;

        uchar pixel = (pixels & (1 << px1));

        if (pixel && screen[pos + px])
          hit = 1;

        screen[pos + px] ^= pixel;
      }

      pos += sw;
    }

    return hit;
  }

  //---

  class IntInRange {
   public:
    IntInRange(int min, int max) :
     eng_(rd_()), idis_(min, max) {
    }

    int gen() {
      return idis_(eng_);
    }

   private:
    std::random_device                 rd_;
    std::default_random_engine         eng_;
    std::uniform_int_distribution<int> idis_;
  };

  uchar rand() {
    IntInRange ir(0, 255);

    return ir.gen();
  }

  //---

  void clearScreen() {
    memset(screen_     , 0, DisplaySize*sizeof(uchar));
    memset(superScreen_, 0, SuperDisplaySize*sizeof(uchar));
  }

 private:
  // Clock Speed 500Hz

  // 4K memory
  //  Interpreter : 0x000 to 0x1FF
  //  Program     : 0x200 to 0xFFF
  uchar memory_[MemSize];

  // 16 general purpose 8 bit registers
  uchar V_[NumV];

  uchar* VF_ = &V_[15];

  ushort I_ { 0 };

  uchar R_[NumV];

  // timer registers
  uchar DT_ { 0 }; // delay timer (active > 0, count down to 0 at 60hz and deactivate)
  uchar ST_ { 0 }; // sound timer (active > 0, count down to 0 at 60hz and deactivate, buzz at > 0)

  // program counter
  ushort PC_ { 0 };

  // stack pointer (offset)
  uchar  SP_ { 0 };
  // stack
  ushort stack_[StackSize];

  // keys:
  //  1 2 3 C
  //  4 5 6 D
  //  7 8 9 E
  //  A 0 B F
  uchar keys_[NumKeys]; // pressed: 1, not pressed 0

  // display
  uchar screen_     [DisplaySize];
  uchar superScreen_[SuperDisplaySize];
  uchar scrollBuffer[DisplaySize];

  // sprites (8x16)
//using Sprite      = uchar  [16];
//using SuperSprite = ushort [16];

//Sprite      sprites_     [16];
//SuperSprite superSprites_[16];

  // config
  bool superChip48_ = false;
  bool highRes_     = false;

  // wait key
  bool  waitKey_    { false };
  uchar waitInd_    { 0 };
  uchar keyPressed_ { 0 };
};

#endif
