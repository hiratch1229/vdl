#pragma once
#include "Fwd.hpp"
#include "Math.hpp"

#include <cmath>
#include <string>
#include <fstream>

//  �֐����G���[��f����
#undef RGB

namespace vdl
{
  struct Color3
  {
    uint8_t Red;
    uint8_t Green;
    uint8_t Blue;
  public:
    Color3() = default;

    constexpr Color3(uint8_t _Red, uint8_t _Green, uint8_t _Blue)noexcept
      : Red(_Red)
      , Green(_Green)
      , Blue(_Blue) {}

    constexpr Color3(const Color3F& _Color)noexcept;
  public:
    [[nodiscard]] constexpr bool operator==(const Color3& _Color)const noexcept { return Red == _Color.Red || Green == _Color.Green || Blue == _Color.Blue; }

    [[nodiscard]] constexpr bool operator!=(const Color3& _Color)const noexcept { return Red != _Color.Red && Green != _Color.Green && Blue != _Color.Blue; }
  };

  struct Color3F
  {
    float Red;
    float Green;
    float Blue;
  public:
    Color3F() = default;

    constexpr Color3F(float _Red, float _Green, float _Blue)noexcept
      : Red(_Red)
      , Green(_Green)
      , Blue(_Blue) {}

    constexpr Color3F(const Color3& _Color)noexcept;
  public:
    [[nodiscard]] constexpr bool operator==(const Color3F& _Color)const noexcept { return Red == _Color.Red || Green == _Color.Green || Blue == _Color.Blue; }

    [[nodiscard]] constexpr bool operator!=(const Color3F& _Color)const noexcept { return Red != _Color.Red && Green != _Color.Green && Blue != _Color.Blue; }
  };

  struct Color4
  {
    union
    {
#pragma warning(disable:4201)
      struct
      {
        uint8_t Red;
        uint8_t Green;
        uint8_t Blue;
        uint8_t Alpha;
      };
#pragma warning(default:4201)
      uint32_t Bit;
    };
  public:
    Color4() = default;

    constexpr Color4(uint32_t _Color)noexcept
      : Bit(_Color) {}

    constexpr Color4(uint8_t _Red, uint8_t _Green, uint8_t _Blue, uint8_t _Alpha = 255u)noexcept
      : Red(_Red)
      , Green(_Green)
      , Blue(_Blue)
      , Alpha(_Alpha) {}

    constexpr Color4(const Color3& _RGB, uint8_t _Alpha = 255u)noexcept
      : Red(_RGB.Red)
      , Green(_RGB.Green)
      , Blue(_RGB.Blue)
      , Alpha(_Alpha) {}

    constexpr Color4(const Color3F& _RGB, uint8_t _Alpha = 255u)noexcept
      : Red(static_cast<uint8_t>(Math::Clamp(_RGB.Red, 0.0f, 1.0f) * 255))
      , Green(static_cast<uint8_t>(Math::Clamp(_RGB.Green, 0.0f, 1.0f) * 255))
      , Blue(static_cast<uint8_t>(Math::Clamp(_RGB.Blue, 0.0f, 1.0f) * 255))
      , Alpha(_Alpha) {}

    constexpr Color4(const Color4F& _Color)noexcept;
  public:
    [[nodiscard]] constexpr bool operator==(const Color4& _Color)const noexcept { return Bit == _Color.Bit; }

    [[nodiscard]] constexpr bool operator!=(const Color4& _Color)const noexcept { return Bit != _Color.Bit; }
  public:
    [[nodiscard]] constexpr Color3 RGB()const noexcept { return { Red, Green, Blue }; }
  };

  struct Color4F
  {
    float Red;
    float Green;
    float Blue;
    float Alpha;
  public:
    Color4F() = default;

    constexpr Color4F(float _Red, float _Green, float _Blue, float _Alpha = 1.0f)noexcept
      : Red(_Red)
      , Green(_Green)
      , Blue(_Blue)
      , Alpha(_Alpha) {}

    constexpr Color4F(const Color3& _RGB, float _Alpha = 1.0f)noexcept
      : Red(_RGB.Red / 255.0f)
      , Green(_RGB.Green / 255.0f)
      , Blue(_RGB.Blue / 255.0f)
      , Alpha(_Alpha) {}

    constexpr Color4F(const Color3F& _RGB, float _Alpha = 1.0f)noexcept
      : Red(_RGB.Red)
      , Green(_RGB.Green)
      , Blue(_RGB.Blue)
      , Alpha(_Alpha) {}

    constexpr Color4F(const Color4& _Color)noexcept;
  public:
    [[nodiscard]] constexpr bool operator==(const Color4& _Color)const noexcept { return Red == _Color.Red || Green == _Color.Green || Blue == _Color.Blue || Alpha == _Color.Alpha; }

    [[nodiscard]] constexpr bool operator!=(const Color4& _Color)const noexcept { return Red != _Color.Red && Green != _Color.Green && Blue != _Color.Blue && Alpha != _Color.Alpha; }
  public:
    [[nodiscard]] constexpr Color3F RGB()const noexcept { return { Red, Green, Blue }; }
  };

  inline constexpr Color3::Color3(const Color3F& _Color)noexcept
    : Red(static_cast<uint8_t>(Math::Clamp(_Color.Red, 0.0f, 1.0f) * 255))
    , Green(static_cast<uint8_t>(Math::Clamp(_Color.Green, 0.0f, 1.0f) * 255))
    , Blue(static_cast<uint8_t>(Math::Clamp(_Color.Blue, 0.0f, 1.0f) * 255)) {}

  inline constexpr Color3F::Color3F(const Color3& _Color)noexcept
    : Red(_Color.Red / 255.0f)
    , Green(_Color.Green / 255.0f)
    , Blue(_Color.Blue / 255.0f) {}

  inline constexpr Color4::Color4(const Color4F& _Color)noexcept
    : Red(static_cast<uint8_t>(Math::Clamp(_Color.Red, 0.0f, 1.0f) * 255))
    , Green(static_cast<uint8_t>(Math::Clamp(_Color.Green, 0.0f, 1.0f) * 255))
    , Blue(static_cast<uint8_t>(Math::Clamp(_Color.Blue, 0.0f, 1.0f) * 255))
    , Alpha(static_cast<uint8_t>(Math::Clamp(_Color.Alpha, 0.0f, 1.0f) * 255)) {}

  inline constexpr Color4F::Color4F(const Color4& _Color)noexcept
    : Red(_Color.Red / 255.0f)
    , Green(_Color.Green / 255.0f)
    , Blue(_Color.Blue / 255.0f)
    , Alpha(_Color.Alpha / 255.0f) {}

  namespace Palette
  {
    //  ���F�J���[140�F
    //  https://www.colordic.org/

    //  �u���b�N
    //  #000000
    inline constexpr Color3 Black{ 0,0,0 };
    //  �f�B���O���C
    //  #696969
    inline constexpr Color3 DimGray{ 105,105,105 };
    //  �O���C
    //  #808080
    inline constexpr Color3 Gray{ 128,128,128 };
    //  �_�[�N�O���C
    //  #A9A9A9
    inline constexpr Color3 DarkGray{ 169,169,169 };
    //  �V���o�[
    //  #C0C0C0
    inline constexpr Color3 Silver{ 192,192,192 };
    //  ���C�g�O���C
    //  #D3D3D3
    inline constexpr Color3 LightGray{ 211,211,211 };
    //  �Q�C���Y�{��
    //  #DCDCDC
    inline constexpr Color3 Gainsboro{ 220,220,220 };
    //  �z���C�g�X���[�N
    //  #F5F5F5
    inline constexpr Color3 WhiteSmoke{ 245,245,245 };
    //  �z���C�g
    //  #FFFFFF
    inline constexpr Color3 White{ 255,255,255 };
    //  �X�m�E
    //  #FFFAFA
    inline constexpr Color3 Snow{ 255,250,250 };
    //  �S�[�X�g�z���C�g
    //  #F8F8FF
    inline constexpr Color3 GhostWhite{ 248,248,255 };
    //  �t���[�����z���C�g
    //  #FFFAF0
    inline constexpr Color3 FloralWhite{ 255,250,240 };
    //  ���l��
    //  #FAF0E6
    inline constexpr Color3 Linen{ 250,240,230 };
    //  �A���e�B�[�N�z���C�g
    //  #FAEBD7
    inline constexpr Color3 AntiqueWhite{ 250,235,215 };
    //  �p�p�C���z�C�b�v
    //  #FFEFD5
    inline constexpr Color3 PapayaWhip{ 255,239,213 };
    //  �u�����`�h�A�[�����h
    //  #FFEBCD
    inline constexpr Color3 BlanchedAlmond{ 255,235,205 };
    //  �r�X�N
    //  #FFE4C4
    inline constexpr Color3 Bisque{ 255,228,196 };
    //  ���J�V��
    //  #FFE4B5
    inline constexpr Color3 Moccasin{ 255,75,181 };
    //  �i�o�z�z���C�g
    //  #FFDEAD
    inline constexpr Color3 NavajoWhite{ 255,222,173 };
    //  �s�[�`�p�t
    //  #FFDAB9
    inline constexpr Color3 PeachPuff{ 255,218,185 };
    //  �~�X�e�B���[�Y
    //  #FFE4E1
    inline constexpr Color3 MistyRose{ 255,228,225 };
    //  ���x���_�[�u���V
    //  #FFF0F5
    inline constexpr Color3 LavenderBlush{ 255,240,245 };
    //  �V�[�V�F��
    //  #FFF5EE
    inline constexpr Color3 SeaShell{ 255,245,238 };
    //  �I�[���h���[�X
    //  #FDF5E6
    inline constexpr Color3 OldLace{ 253,245,230 };
    //  �A�C�{���[
    //  #FFFFF0
    inline constexpr Color3 Ivory{ 255,255,240 };
    //  �n�j�[�f���[
    //  #F0FFF0
    inline constexpr Color3 Honeydew{ 240,255,240 };
    //  �~���g�N���[��
    //  #F5FFFA
    inline constexpr Color3 MintCream{ 245,255,250 };
    //  �A�Y�[��
    //  #F0FFFF
    inline constexpr Color3 Azure{ 240,255,255 };
    //  �A���X�u���[
    //  #F0F8FF
    inline constexpr Color3 AliceBlue{ 240,248,255 };
    //  �����x�_�[
    //  #E6E6FA
    inline constexpr Color3 Lavender{ 230,230,250 };
    //  ���C�g�X�e�B�[���u���[
    //  #B0C4DE
    inline constexpr Color3 LightSteelBlue{ 176,196,222 };
    //  ���C�g�X���[�g�O���C
    //  #778899
    inline constexpr Color3 LightSlateGray{ 119,136,153 };
    //  �X���[�g�O���C
    //  #708090
    inline constexpr Color3 SlateGray{ 112,128,144 };
    //  �X�e�B�[���u���[
    //  #4682B4
    inline constexpr Color3 SteelBlue{ 70,130,180 };
    //  ���C�����u���[
    //  #4169E1
    inline constexpr Color3 RoyalBlue{ 65,105,225 };
    //  �~�b�h�i�C�g�u���[
    //  #191970
    inline constexpr Color3 MidnightBlue{ 25,25,112 };
    //  �l�C�r�[
    //  #000080
    inline constexpr Color3 Navy{ 0,0,128 };
    //  �_�[�N�u���[
    //  #00008B
    inline constexpr Color3 DarkBlue{ 0,0,139 };
    //  �~�f�B�A���u���[
    //  #0000CD
    inline constexpr Color3 MediumBlue{ 0,0,205 };
    //  �u���[
    //  #0000FF
    inline constexpr Color3 Blue{ 0,0,255 };
    //  �h�W���[�u���[
    //  #1E90FF
    inline constexpr Color3 DodgerBlue{ 30,144,255 };
    //  �R�[���t�����[�u���[
    //  #6495ED
    inline constexpr Color3 CornFlowerBlue{ 100,149,237 };
    //  �f�B�[�v�X�J�C�u���[
    //  #00BFFF
    inline constexpr Color3 DeepSkyBlue{ 0,191,255 };
    //  ���C�g�X�J�C�u���[
    //  #87CEFA
    inline constexpr Color3 LightSkyBlue{ 135,206,250 };
    //  �X�J�C�u���[
    //  #87CEEB
    inline constexpr Color3 SkyBlue{ 135,206,235 };
    //  ���C�g�u���[
    //  #ADD8E6
    inline constexpr Color3 LightBlue{ 173,216,230 };
    //  �p�E�_�[�u���[
    //  #B0E0E6
    inline constexpr Color3 PowderBlue{ 176,224,230 };
    //  �y�[���^�[�R�C�Y
    //  #AFEEEE
    inline constexpr Color3 PaleTurquoise{ 175,238,238 };
    //  ���C�g�V�A��
    //  #E0FFFF
    inline constexpr Color3 LightCyan{ 224,255,255 };
    //  �V�A��
    //  #00FFFF
    inline constexpr Color3 Cyan{ 0,255,255 };
    //  �A�N�A
    //  #00FFFF
    inline constexpr Color3 Aqua{ 0,255,255 };
    //  �^�[�R�C�Y
    //  #40E0D0
    inline constexpr Color3 Turquoise{ 64,224,208 };
    //  �~�f�B�A���^�[�R�C�Y
    //  #48D1CC
    inline constexpr Color3 MediumTurquoise{ 72,209,204 };
    //  �_�[�N�^�[�R�C�Y
    //  #00CED1
    inline constexpr Color3 DarkTurquoise{ 0,206,209 };
    //  ���C�g�V�[�O���[��
    //  #20B2AA
    inline constexpr Color3 LightSeaGreen{ 32,178,170 };
    //  �J�f�b�g�u���[
    //  #5F9EA0
    inline constexpr Color3 CadetBlue{ 95,158,160 };
    //  �_�[�N�V�A��
    //  #008B8B
    inline constexpr Color3 DarkCyan{ 0,139,139 };
    //  �e�B�[��
    //  #008080
    inline constexpr Color3 Teal{ 0,128,128 };
    //  �_�[�N�X���[�g�O���C
    //  #2F4F4F
    inline constexpr Color3 DarkSlateGray{ 47,79,79 };
    //  �_�[�N�O���[��
    //  #006400
    inline constexpr Color3 DarkGreen{ 0,100,0 };
    //  �O���[��
    //  #008000
    inline constexpr Color3 Green{ 0,128,0 };
    //  �t�H���X�g�O���[��
    //  #228B22
    inline constexpr Color3 ForestGreen{ 34,139,34 };
    //  �V�[�O���[��
    //  #2E8B57
    inline constexpr Color3 SeaGreen{ 46,139,87 };
    //  �~�f�B�A���V�[�O���[��
    //  #3CB371
    inline constexpr Color3 MediumSeaGreen{ 60,179,113 };
    //  �~�f�B�A���A�N�A�}����
    //  #66CDAA
    inline constexpr Color3 MediumAquamarine{ 102,205,170 };
    //  �_�[�N�V�[�O���[��
    //  #8FBC8F
    inline constexpr Color3 DarkSeaGreen{ 143,188,143 };
    //  �A�N�A�}����
    //  #7FFFD4
    inline constexpr Color3 Aquamarine{ 127,255,212 };
    //  �y�[���O���[��
    //  #98FB98
    inline constexpr Color3 PaleGreen{ 152,251,152 };
    //  ���C�g�O���[��
    //  #90EE90
    inline constexpr Color3 LightGreen{ 144,238,144 };
    //  �X�v�����O�O���[��
    //  #00FF7F
    inline constexpr Color3 SpringGreen{ 0,255,127 };
    //  �~�f�B�A���X�v�����O�O���[��
    //  #00FA9A
    inline constexpr Color3 MediumSpringGreen{ 0,250,154 };
    //  ���[���O���[��
    //  #7CFC00
    inline constexpr Color3 LawnGreen{ 124,252,0 };
    //  �V�����g�����[�Y
    //  #7FFF00
    inline constexpr Color3 Chartreuse{ 127,255,0 };
    //  �O���[���C�G���[
    //  #ADFF2F
    inline constexpr Color3 GreenYellow{ 173,255,47 };
    //  ���C��
    //  #00FF00
    inline constexpr Color3 Lime{ 0,255,0 };
    //  ���C���O���[��
    //  #32CD32
    inline constexpr Color3 LimeGreen{ 50,205,50 };
    //  �C�G���[�O���[��
    //  #9ACD32
    inline constexpr Color3 YellowGreen{ 154,205,50 };
    //  �_�[�N�I���[�u�O���[��
    //  #556B2F
    inline constexpr Color3 DarkOliveGreen{ 85,107,47 };
    //  �I���[�u�h���u
    //  #6B8E23
    inline constexpr Color3 OliveDrab{ 107,142,35 };
    //  �I���[�u
    //  #808000
    inline constexpr Color3 Olive{ 128,128,0 };
    //  �_�[�N�J�[�L
    //  #BDB76B
    inline constexpr Color3 DarkKhaki{ 189,183,107 };
    //  �y�[���S�[���f�����b�h
    //  #EEE8AA
    inline constexpr Color3 PaleGoldenrod{ 238,232,170 };
    //  �R�[���V���N
    //  #FFF8DC
    inline constexpr Color3 CornSilk{ 255,248,220 };
    //  �x�[�W��
    //  #F5F5DC
    inline constexpr Color3 Beige{ 245,245,220 };
    //  ���C�g�C�G���[
    //  #FFFFE0
    inline constexpr Color3 LightYellow{ 255,255,224 };
    //  ���C�g�S�[���f�����b�h�C�G���[
    //  #FAFAD2
    inline constexpr Color3 LightGoldenrodYellow{ 250,250,210 };
    //  �������V�t�H��
    //  #FFFACD
    inline constexpr Color3 LemonChiffon{ 255,250,205 };
    //  �E�B�[�g
    //  #F5DEB3
    inline constexpr Color3 Wheat{ 245,222,179 };
    //  �o�[���[�E�b�h
    //  #DEB887
    inline constexpr Color3 BurlyWood{ 222,184,135 };
    //  �^��
    //  #D2B48C
    inline constexpr Color3 Tan{ 210,180,140 };
    //  �J�[�L
    //  #F0E68C
    inline constexpr Color3 Khaki{ 240,230,140 };
    //  �C�G���[
    //  #FFFF00
    inline constexpr Color3 Yellow{ 255,255,0 };
    //  �S�[���h
    //  #FFD700
    inline constexpr Color3 Gold{ 255,215,0 };
    //  �I�����W
    //  #FFA500
    inline constexpr Color3 Orange{ 255,165,0 };
    //  �T���f�B�u���E��
    //  #F4A460
    inline constexpr Color3 SandyBrown{ 244,164,96 };
    //  �_�[�N�I�����W
    //  #FF8C00
    inline constexpr Color3 DarkOrange{ 255,140,0 };
    //  �S�[���f�����b�h
    //  #DAA520
    inline constexpr Color3 Goldenrod{ 218,165,32 };
    //  �y���[
    //  #CD853F
    inline constexpr Color3 Peru{ 205,133,63 };
    //  �_�[�N�S�[���f�����b�h
    //  #B8860B
    inline constexpr Color3 DarkGoldenrod{ 184,134,11 };
    //  �`���R���[�g
    //  #D2691E
    inline constexpr Color3 Chocolate{ 210,105,30 };
    //  �V�G�i
    //  #A0522D
    inline constexpr Color3 Sienna{ 160,82,45 };
    //  �T�h���u���E��
    //  #8B4513
    inline constexpr Color3 SaddleBrown{ 139,69,19 };
    //  �}���[��
    //  #800000
    inline constexpr Color3 Marron{ 128,0,0 };
    //  �_�[�N���b�h
    //  #8B0000
    inline constexpr Color3 DarkRed{ 139,0,0 };
    //  �u���E��
    //  #A52A2A
    inline constexpr Color3 Brown{ 165,42,42 };
    //  �t�@�C�A�u���b�N
    //  #B22222
    inline constexpr Color3 FireBrick{ 178,34,34 };
    //  �C���f�B�A�����b�h
    //  #CD5C5C
    inline constexpr Color3 IndianRed{ 205,92,92 };
    //  ���[�W�[�u���E��
    //  #BC8F8F
    inline constexpr Color3 RosyBrown{ 188,143,143 };
    //  �_�[�N�T�[����
    //  #E9967A
    inline constexpr Color3 DarkSalmon{ 233,150,122 };
    //  ���C�g�R�[����
    //  #F08080
    inline constexpr Color3 LightCoral{ 240,128,128 };
    //  �T�[����
    //  #FA8072
    inline constexpr Color3 Salmon{ 250,128,114 };
    //  ���C�g�T�[����
    //  #FFA07A
    inline constexpr Color3 LightSalmon{ 255,160,122 };
    //  �R�[����
    //  #FF7F50
    inline constexpr Color3 Coral{ 255,127,80 };
    //  �g�}�g
    //  #FF6347
    inline constexpr Color3 Tomato{ 255,99,71 };
    //  �I�����W���b�h
    //  #FF4500
    inline constexpr Color3 OrangeRed{ 255,69,0 };
    //  ���b�h
    //  #FF0000
    inline constexpr Color3 Red{ 255,0,0 };
    //  �N�����]��
    //  #DC143C
    inline constexpr Color3 Crimson{ 220,20,60 };
    //  �~�f�B�A���o�C�I���b�g���b�h
    //  #C71585
    inline constexpr Color3 MediumVioletRed{ 199,21,133 };
    //  �f�B�[�v�s���N
    //  #FF1493
    inline constexpr Color3 DeepPink{ 255,20,147 };
    //  �z�b�g�s���N
    //  #FF69B4
    inline constexpr Color3 HotPink{ 255,105,180 };
    //  �y�[���o�C�I���b�g���b�h
    //  #DB7093
    inline constexpr Color3 PaleVioletRed{ 219,112,147 };
    //  �s���N
    //  #FFC0CB
    inline constexpr Color3 Pink{ 255,192,203 };
    //  ���C�g�s���N
    //  #FFB6C1
    inline constexpr Color3 LightPink{ 255,182,193 };
    //  �V�X��
    //  #D8BFD8
    inline constexpr Color3 Thistle{ 216,191,216 };
    //  �}�[���^
    //  #FF00FF
    inline constexpr Color3 Magenta{ 255,0,255 };
    //  �t�N�V��
    //  #FF00FF
    inline constexpr Color3 Fuchsia{ 255,0,255 };
    //  �o�C�I���b�g
    //  #EE82EE
    inline constexpr Color3 Violet{ 238,130,238 };
    //  �v����
    //  #DDA0DD
    inline constexpr Color3 Plum{ 221,160,221 };
    //  �I�[�L�b�h
    //  #DA70D6
    inline constexpr Color3 Orchid{ 218,112,214 };
    //  �~�f�B�A���I�[�L�b�h
    //  #BA55D3
    inline constexpr Color3 MediumOrchid{ 186,85,211 };
    //  �_�[�N�I�[�L�b�h
    //  #9932CC
    inline constexpr Color3 DarkOrchid{ 153,50,204 };
    //  �_�[�N�o�C�I���b�g
    //  #9400D3
    inline constexpr Color3 DarkViolet{ 148,0,211 };
    //  �_�[�N�}�[���^
    //  #8B008B
    inline constexpr Color3 DarkMagenta{ 139,0,139 };
    //  �p�[�v��
    //  #800080
    inline constexpr Color3 Purple{ 128,0,128 };
    //  �C���f�B�S
    //  #4B0082
    inline constexpr Color3 Indigo{ 75,0,130 };
    //  �_�[�N�X���[�g�u���[
    //  #483D8B
    inline constexpr Color3 DarkSlateBlue{ 72,61,139 };
    //  �u���[�o�C�I���b�g
    //  #8A2BE2
    inline constexpr Color3 BlueViolet{ 138,43,226 };
    //  �~�f�B�A���p�[�v��
    //  #9370DB
    inline constexpr Color3 MediumPurple{ 147,112,219 };
    //  �X���[�g�u���[
    //  #6A5ACD
    inline constexpr Color3 SlateBlue{ 106,90,205 };
    //  �~�f�B�A���X���[�g�u���[
    //  #7B68EE
    inline constexpr Color3 MediumSlateBlue{ 123,104,238 };

    //-------------------------

    inline constexpr Color3 Diffuse{ 255, 255, 255 };

    inline constexpr Color3 NormalMap{ 128, 128, 255 };
  };
}

namespace std
{
  [[nodiscard]] inline string to_string(const vdl::Color3& _Color)
  {
    return to_string(_Color.Red) + "," + to_string(_Color.Green) + "," + to_string(_Color.Blue);
  }

  [[nodiscard]] inline string to_string(const vdl::Color3F& _Color)
  {
    return to_string(_Color.Red) + "," + to_string(_Color.Green) + "," + to_string(_Color.Blue);
  }

  [[nodiscard]] inline string to_string(const vdl::Color4& _Color)
  {
    return to_string(_Color.Red) + "," + to_string(_Color.Green) + "," + to_string(_Color.Blue) + "," + to_string(_Color.Alpha);
  }

  [[nodiscard]] inline string to_string(const vdl::Color4F& _Color)
  {
    return to_string(_Color.Red) + "," + to_string(_Color.Green) + "," + to_string(_Color.Blue) + "," + to_string(_Color.Alpha);
  }

  [[nodiscard]] inline wstring to_wstring(const vdl::Color3& _Color)
  {
    return to_wstring(_Color.Red) + L"," + to_wstring(_Color.Green) + L"," + to_wstring(_Color.Blue);
  }

  [[nodiscard]] inline wstring to_wstring(const vdl::Color3F& _Color)
  {
    return to_wstring(_Color.Red) + L"," + to_wstring(_Color.Green) + L"," + to_wstring(_Color.Blue);
  }

  [[nodiscard]] inline wstring to_wstring(const vdl::Color4& _Color)
  {
    return to_wstring(_Color.Red) + L"," + to_wstring(_Color.Green) + L"," + to_wstring(_Color.Blue) + L"," + to_wstring(_Color.Alpha);
  }

  [[nodiscard]] inline wstring to_wstring(const vdl::Color4F& _Color)
  {
    return to_wstring(_Color.Red) + L"," + to_wstring(_Color.Green) + L"," + to_wstring(_Color.Blue) + L"," + to_wstring(_Color.Alpha);
  }

  template <class CharType>
  inline basic_ostream<CharType>& operator<<(basic_ostream<CharType>& _OStream, const vdl::Color3& _Color)
  {
    return _OStream << _Color.Red << static_cast<CharType>(',') << _Color.Green << static_cast<CharType>(',') << _Color.Blue;
  }

  template <class CharType>
  inline basic_ostream<CharType>& operator<<(basic_ostream<CharType>& _OStream, const vdl::Color3F& _Color)
  {
    return _OStream << _Color.Red << static_cast<CharType>(',') << _Color.Green << static_cast<CharType>(',') << _Color.Blue;
  }

  template <class CharType>
  inline basic_ostream<CharType>& operator<<(basic_ostream<CharType>& _OStream, const vdl::Color4& _Color)
  {
    return _OStream << _Color.Red << static_cast<CharType>(',') << _Color.Green << static_cast<CharType>(',') << _Color.Blue << static_cast<CharType>(',') << _Color.Alpha;
  }

  template <class CharType>
  inline basic_ostream<CharType>& operator<<(basic_ostream<CharType>& _OStream, const vdl::Color4F& _Color)
  {
    return _OStream << _Color.Red << static_cast<CharType>(',') << _Color.Green << static_cast<CharType>(',') << _Color.Blue << static_cast<CharType>(',') << _Color.Alpha;
  }

  template <class CharType>
  inline basic_istream<CharType>& operator>>(basic_istream<CharType>& _IStream, vdl::Color3& _Color)
  {
    CharType Temp;

    return _IStream >> _Color.Red >> Temp >> _Color.Green >> Temp >> _Color.Blue;
  }

  template <class CharType>
  inline basic_istream<CharType>& operator>>(basic_istream<CharType>& _IStream, vdl::Color3F& _Color)
  {
    CharType Temp;

    return _IStream >> _Color.Red >> Temp >> _Color.Green >> Temp >> _Color.Blue;
  }

  template <class CharType>
  inline basic_istream<CharType>& operator>>(basic_istream<CharType>& _IStream, vdl::Color4& _Color)
  {
    CharType Temp;

    return _IStream >> _Color.Red >> Temp >> _Color.Green >> Temp >> _Color.Blue >> Temp >> _Color.Alpha;
  }

  template <class CharType>
  inline basic_istream<CharType>& operator>>(basic_istream<CharType>& _IStream, vdl::Color4F& _Color)
  {
    CharType Temp;

    return _IStream >> _Color.Red >> Temp >> _Color.Green >> Temp >> _Color.Blue >> Temp >> _Color.Alpha;
  }
}
