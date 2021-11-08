#pragma once

#include <array>
#include <functional>

#include "MainBus.h"
#include "PaletteColors.h"
#include "PictureBus.h"
#include "VirtualScreen.h"

#define PPUSTATUS_IN_BYTE
#define PPUCONTROL_IN_BYTE
#define PPUMASK_IN_BYTE
namespace hn {
constexpr int ScanlineEndCycle = 340;
constexpr int VisibleScanlines = 240;
constexpr int ScanlineVisibleDots = 256;

class PPU {
 public:
  PPU(PictureBus &bus, VirtualScreen &screen);
  void Step();
  void Reset();

  void setInterruptCallback(std::function<void(void)> cb);

  void doDMA(const Byte *page_ptr);

  // Callbacks mapped to CPU address space
  // Addresses written to by the program
  void control(Byte ctrl);
  void setMask(Byte mask);
  void setOAMAddress(Byte addr);
  void setDataAddress(Byte addr);
  void setScroll(Byte scroll);
  void setData(Byte data);
  // Read by the program
  Byte getStatus();
  Byte getData();
  Byte getOAMData();
  void setOAMData(Byte value);

  void DebugDump();

  PictureBus &bus() const { return bus_; }

 protected:
  void postRender();
  void preRender();
  void render();
  void vBlank();

  void renderInScanline();

 private:
  Byte read(Address addr);

  PictureBus &bus_;
  VirtualScreen &screen_;

  std::function<void(void)> vblankCallback_;

  enum State { PreRender, Render, PostRender, VerticalBlank } pipelineState_;
  int cycle_;
  int scanline_;
  bool evenFrame_;

  // Registers
  Address dataAddress_;
  Address tempAddress_;
  Address dataAddrIncrement_;

  bool firstWrite_;
  Byte fineXScroll_;
  Byte dataBuffer_;

  Byte oamDataAddress_;

  struct {
    Byte y;
    Byte tile;
    Byte attr;
    Byte x;
  } Sprites_[64];

  // Setup flags and variables
  // Control byte
#ifdef PPUCONTROL_IN_BYTE
  Byte ppu_control_;
#else   // PPUCONTROL_IN_BYTE
  bool longSprites_;
  bool generateInterrupt_;

  enum CharacterPage {
    Low,
    High,
  } bgPage_,
      sprPage_;
#endif  // PPUCONTROL_IN_BYTE

  // Mask byte
#ifdef PPUMASK_IN_BYTE
  Byte ppu_mask_;
#else   // PPUMASK_IN_BYTE
  bool greyscaleMode_;
  bool showSprites_;
  bool showBackground_;
  bool showEdgeSprites_;
  bool showEdgeBackground_;
#endif  // PPUMASK_IN_BYTE

  // Status byte
#ifdef PPUSTATUS_IN_BYTE
  Byte ppu_status_;
#else   // PPUSTATUS_IN_BYTE
  bool vblank_;
  bool sprZeroHit_;
#endif  // PPUSTATUS_IN_BYTE

  std::size_t frameIndex_;
  std::vector<Byte> spriteMemory_;
  std::vector<Byte> scanlineSprites_;
  std::vector<std::vector<sf::Color>> pictureBuffer_;
};

}  // namespace hn