#pragma once
#include "Fwd.hpp"
#include "Types.hpp"

#include <cmath>
#include <string>
#include <fstream>

namespace vdl
{
  //  �J���[
  //  R,G,B,A ���ꂼ��0~255�ŐF��\�����܂�
  struct Color
  {
#pragma warning(disable:4201)
    union
    {
      struct
      {
        uint8_t R; //  ��
        uint8_t G; //  ��
        uint8_t B; //  ��
        uint8_t A; //  �s�����x
      };
      uint32_t Bit;
    };
#pragma warning(default:4201)
  public:
    Color() = default;

    //  �R�s�[�R���X�g���N�^
    //  _Color:�R�s�[����F
    constexpr Color(const Color& _Color)noexcept : R(_Color.R), G(_Color.G), B(_Color.B), A(_Color.A) {}
    //  �F��ϊ����ăR�s�[
    //  _Color:�R�s�[����F
    constexpr Color(const ColorF& _Color)noexcept;
    //  �F���쐬
    //  _R:��
    //  _G:��
    //  _B:��
    //  _A:�s�����x
    constexpr Color(uint32_t _R, uint32_t _G, uint32_t _B, uint32_t _A = 255u)noexcept : R(_R), G(_G), B(_B), A(_A) {}
    //  �F���쐬
    //  _RGB:��,��,��
    //  _A:�s�����x
    constexpr Color(uint32_t _RGB, uint32_t _A = 255u)noexcept : R(_RGB), G(_RGB), B(_RGB), A(_A) {}
    //  �F���쐬
    //  _RGB:��,��,��
    //  _A:�s�����x
    constexpr Color(const Color& _RGB, uint32_t _A)noexcept : R(_RGB.R), G(_RGB.G), B(_RGB.B), A(_A) {}
  public:
    //  �F����
    //  _Color:�������F
    Color& operator =(const Color& _Color)noexcept
    {
      R = _Color.R;
      G = _Color.G;
      B = _Color.B;
      A = _Color.A;

      return *this;
    }
    //  �F�����������𔻒f
    //  _Color:��r�Ώۂ̐F
    //  (true:������,false:�������Ȃ�)
    constexpr bool operator ==(const Color& _Color)const noexcept
    {
      return (R == _Color.R && G == _Color.G && B == _Color.B && A == _Color.A);
    }
    //  �F�����������𔻒f
    //  _Color:��r�Ώۂ̐F
    //  (true:�������Ȃ�,false:������)
    constexpr bool operator !=(const Color& _Color)const noexcept
    {
      return !(*this == _Color);
    }
  };

  //  �J���[
  //  R,G,B,A ���ꂼ��0~1�ŐF��\�����܂�
  struct ColorF
  {
    float R;  //  ��
    float G;  //  ��
    float B;  //  ��
    float A;  //  �s�����x
  public:
    ColorF() = default;

    //  �R�s�[�R���X�g���N�^
    //  _Color:�R�s�[����F
    constexpr ColorF(const ColorF& _Color)noexcept : R(_Color.R), G(_Color.G), B(_Color.B), A(_Color.A) {}
    //  �F��ϊ����ăR�s�[
    //  _Color:�R�s�[����F
    constexpr ColorF(const Color& _Color)noexcept : R(_Color.R / 255.0f), G(_Color.G / 255.0f), B(_Color.B / 255.0f), A(_Color.A / 255.0f) {}
    //  �F���쐬
    //  _R:��
    //  _G:��
    //  _B:��
    //  _A:�s�����x
    constexpr ColorF(float _R, float _G, float _B, float _A = 1.0f)noexcept : R(_R), G(_G), B(_B), A(_A) {}
    //  �F���쐬
    //  _RGB:��,��,��
    //  _A:�s�����x
    constexpr ColorF(float _RGB, float _A = 1.0f)noexcept : R(_RGB), G(_RGB), B(_RGB), A(_A) {}
    //  �F���쐬
    //  _RGB:��,��,��
    //  _A:�s�����x
    constexpr ColorF(const ColorF& _RGB, float _A)noexcept : R(_RGB.R), G(_RGB.G), B(_RGB.B), A(_A) {}
  public:
    //  �F��ϊ����đ��
    //  _Color:�������F
    ColorF& operator =(const ColorF& _Color)noexcept
    {
      R = _Color.R;
      G = _Color.G;
      B = _Color.B;
      A = _Color.A;

      return *this;
    }
    //  �F�����������𔻒f
    //  _Color:��r�Ώۂ̐F
    //  (true:������,false:�������Ȃ�)
    constexpr bool operator ==(const ColorF& _Color)const noexcept
    {
      return (R == _Color.R && G == _Color.G && B == _Color.B && A == _Color.A);
    }
    //  �F�����������𔻒f
    //  _Color:��r�Ώۂ̐F
    //  (true:�������Ȃ�,false:������)
    constexpr bool operator !=(const ColorF& _Color)const noexcept
    {
      return !(*this == _Color);
    }
  public:
    constexpr ColorF operator*(const ColorF& _Color)const
    {
      return { R * _Color.R, G * _Color.G, B * _Color.B, A * _Color.A };
    }

    ColorF& operator*=(const ColorF& _Color)
    {
      R *= _Color.R;
      B *= _Color.B;
      G *= _Color.G;
      A *= _Color.A;

      return *this;
    }
  };

  //  �F��ϊ����ăR�s�[
  //  _Color:�R�s�[����F
  inline constexpr Color::Color(const ColorF& _Color)noexcept
    : R(static_cast<uint8_t>(_Color.R * 255.0f)), G(static_cast<uint8_t>(_Color.G * 255.0f)), B(static_cast<uint8_t>(_Color.B * 255.0f)), A(static_cast<uint8_t>(_Color.A * 255.0f))
  {

  }

  //  ���F�J���[140�F
  namespace Palette
  {
    //  �F���{
    //  https://www.colordic.org/

    //  �u���b�N
    //  #000000
    constexpr Color Black{ 0,0,0 };
    //  �f�B���O���C
    //  #696969
    constexpr Color DimGray{ 105,105,105 };
    //  �O���C
    //  #808080
    constexpr Color Gray{ 128,128,128 };
    //  �_�[�N�O���C
    //  #A9A9A9
    constexpr Color DarkGray{ 169,169,169 };
    //  �V���o�[
    //  #C0C0C0
    constexpr Color Silver{ 192,192,192 };
    //  ���C�g�O���C
    //  #D3D3D3
    constexpr Color LightGray{ 211,211,211 };
    //  �Q�C���Y�{��
    //  #DCDCDC
    constexpr Color Gainsboro{ 220,220,220 };
    //  �z���C�g�X���[�N
    //  #F5F5F5
    constexpr Color WhiteSmoke{ 245,245,245 };
    //  �z���C�g
    //  #FFFFFF
    constexpr Color White{ 255,255,255 };
    //  �X�m�E
    //  #FFFAFA
    constexpr Color Snow{ 255,250,250 };
    //  �S�[�X�g�z���C�g
    //  #F8F8FF
    constexpr Color GhostWhite{ 248,248,255 };
    //  �t���[�����z���C�g
    //  #FFFAF0
    constexpr Color FloralWhite{ 255,250,240 };
    //  ���l��
    //  #FAF0E6
    constexpr Color Linen{ 250,240,230 };
    //  �A���e�B�[�N�z���C�g
    //  #FAEBD7
    constexpr Color AntiqueWhite{ 250,235,215 };
    //  �p�p�C���z�C�b�v
    //  #FFEFD5
    constexpr Color PapayaWhip{ 255,239,213 };
    //  �u�����`�h�A�[�����h
    //  #FFEBCD
    constexpr Color BlanchedAlmond{ 255,235,205 };
    //  �r�X�N
    //  #FFE4C4
    constexpr Color Bisque{ 255,228,196 };
    //  ���J�V��
    //  #FFE4B5
    constexpr Color Moccasin{ 255,75,181 };
    //  �i�o�z�z���C�g
    //  #FFDEAD
    constexpr Color NavajoWhite{ 255,222,173 };
    //  �s�[�`�p�t
    //  #FFDAB9
    constexpr Color PeachPuff{ 255,218,185 };
    //  �~�X�e�B���[�Y
    //  #FFE4E1
    constexpr Color MistyRose{ 255,228,225 };
    //  ���x���_�[�u���V
    //  #FFF0F5
    constexpr Color LavenderBlush{ 255,240,245 };
    //  �V�[�V�F��
    //  #FFF5EE
    constexpr Color SeaShell{ 255,245,238 };
    //  �I�[���h���[�X
    //  #FDF5E6
    constexpr Color OldLace{ 253,245,230 };
    //  �A�C�{���[
    //  #FFFFF0
    constexpr Color Ivory{ 255,255,240 };
    //  �n�j�[�f���[
    //  #F0FFF0
    constexpr Color Honeydew{ 240,255,240 };
    //  �~���g�N���[��
    //  #F5FFFA
    constexpr Color MintCream{ 245,255,250 };
    //  �A�Y�[��
    //  #F0FFFF
    constexpr Color Azure{ 240,255,255 };
    //  �A���X�u���[
    //  #F0F8FF
    constexpr Color AliceBlue{ 240,248,255 };
    //  �����x�_�[
    //  #E6E6FA
    constexpr Color Lavender{ 230,230,250 };
    //  ���C�g�X�e�B�[���u���[
    //  #B0C4DE
    constexpr Color LightSteelBlue{ 176,196,222 };
    //  ���C�g�X���[�g�O���C
    //  #778899
    constexpr Color LightSlateGray{ 119,136,153 };
    //  �X���[�g�O���C
    //  #708090
    constexpr Color SlateGray{ 112,128,144 };
    //  �X�e�B�[���u���[
    //  #4682B4
    constexpr Color SteelBlue{ 70,130,180 };
    //  ���C�����u���[
    //  #4169E1
    constexpr Color RoyalBlue{ 65,105,225 };
    //  �~�b�h�i�C�g�u���[
    //  #191970
    constexpr Color MidnightBlue{ 25,25,112 };
    //  �l�C�r�[
    //  #000080
    constexpr Color Navy{ 0,0,128 };
    //  �_�[�N�u���[
    //  #00008B
    constexpr Color DarkBlue{ 0,0,139 };
    //  �~�f�B�A���u���[
    //  #0000CD
    constexpr Color MediumBlue{ 0,0,205 };
    //  �u���[
    //  #0000FF
    constexpr Color Blue{ 0,0,255 };
    //  �h�W���[�u���[
    //  #1E90FF
    constexpr Color DodgerBlue{ 30,144,255 };
    //  �R�[���t�����[�u���[
    //  #6495ED
    constexpr Color CornFlowerBlue{ 100,149,237 };
    //  �f�B�[�v�X�J�C�u���[
    //  #00BFFF
    constexpr Color DeepSkyBlue{ 0,191,255 };
    //  ���C�g�X�J�C�u���[
    //  #87CEFA
    constexpr Color LightSkyBlue{ 135,206,250 };
    //  �X�J�C�u���[
    //  #87CEEB
    constexpr Color SkyBlue{ 135,206,235 };
    //  ���C�g�u���[
    //  #ADD8E6
    constexpr Color LightBlue{ 173,216,230 };
    //  �p�E�_�[�u���[
    //  #B0E0E6
    constexpr Color PowderBlue{ 176,224,230 };
    //  �y�[���^�[�R�C�Y
    //  #AFEEEE
    constexpr Color PaleTurquoise{ 175,238,238 };
    //  ���C�g�V�A��
    //  #E0FFFF
    constexpr Color LightCyan{ 224,255,255 };
    //  �V�A��
    //  #00FFFF
    constexpr Color Cyan{ 0,255,255 };
    //  �A�N�A
    //  #00FFFF
    constexpr Color Aqua{ 0,255,255 };
    //  �^�[�R�C�Y
    //  #40E0D0
    constexpr Color Turquoise{ 64,224,208 };
    //  �~�f�B�A���^�[�R�C�Y
    //  #48D1CC
    constexpr Color MediumTurquoise{ 72,209,204 };
    //  �_�[�N�^�[�R�C�Y
    //  #00CED1
    constexpr Color DarkTurquoise{ 0,206,209 };
    //  ���C�g�V�[�O���[��
    //  #20B2AA
    constexpr Color LightSeaGreen{ 32,178,170 };
    //  �J�f�b�g�u���[
    //  #5F9EA0
    constexpr Color CadetBlue{ 95,158,160 };
    //  �_�[�N�V�A��
    //  #008B8B
    constexpr Color DarkCyan{ 0,139,139 };
    //  �e�B�[��
    //  #008080
    constexpr Color Teal{ 0,128,128 };
    //  �_�[�N�X���[�g�O���C
    //  #2F4F4F
    constexpr Color DarkSlateGray{ 47,79,79 };
    //  �_�[�N�O���[��
    //  #006400
    constexpr Color DarkGreen{ 0,100,0 };
    //  �O���[��
    //  #008000
    constexpr Color Green{ 0,128,0 };
    //  �t�H���X�g�O���[��
    //  #228B22
    constexpr Color ForestGreen{ 34,139,34 };
    //  �V�[�O���[��
    //  #2E8B57
    constexpr Color SeaGreen{ 46,139,87 };
    //  �~�f�B�A���V�[�O���[��
    //  #3CB371
    constexpr Color MediumSeaGreen{ 60,179,113 };
    //  �~�f�B�A���A�N�A�}����
    //  #66CDAA
    constexpr Color MediumAquamarine{ 102,205,170 };
    //  �_�[�N�V�[�O���[��
    //  #8FBC8F
    constexpr Color DarkSeaGreen{ 143,188,143 };
    //  �A�N�A�}����
    //  #7FFFD4
    constexpr Color Aquamarine{ 127,255,212 };
    //  �y�[���O���[��
    //  #98FB98
    constexpr Color PaleGreen{ 152,251,152 };
    //  ���C�g�O���[��
    //  #90EE90
    constexpr Color LightGreen{ 144,238,144 };
    //  �X�v�����O�O���[��
    //  #00FF7F
    constexpr Color SpringGreen{ 0,255,127 };
    //  �~�f�B�A���X�v�����O�O���[��
    //  #00FA9A
    constexpr Color MediumSpringGreen{ 0,250,154 };
    //  ���[���O���[��
    //  #7CFC00
    constexpr Color LawnGreen{ 124,252,0 };
    //  �V�����g�����[�Y
    //  #7FFF00
    constexpr Color Chartreuse{ 127,255,0 };
    //  �O���[���C�G���[
    //  #ADFF2F
    constexpr Color GreenYellow{ 173,255,47 };
    //  ���C��
    //  #00FF00
    constexpr Color Lime{ 0,255,0 };
    //  ���C���O���[��
    //  #32CD32
    constexpr Color LimeGreen{ 50,205,50 };
    //  �C�G���[�O���[��
    //  #9ACD32
    constexpr Color YellowGreen{ 154,205,50 };
    //  �_�[�N�I���[�u�O���[��
    //  #556B2F
    constexpr Color DarkOliveGreen{ 85,107,47 };
    //  �I���[�u�h���u
    //  #6B8E23
    constexpr Color OliveDrab{ 107,142,35 };
    //  �I���[�u
    //  #808000
    constexpr Color Olive{ 128,128,0 };
    //  �_�[�N�J�[�L
    //  #BDB76B
    constexpr Color DarkKhaki{ 189,183,107 };
    //  �y�[���S�[���f�����b�h
    //  #EEE8AA
    constexpr Color PaleGoldenrod{ 238,232,170 };
    //  �R�[���V���N
    //  #FFF8DC
    constexpr Color CornSilk{ 255,248,220 };
    //  �x�[�W��
    //  #F5F5DC
    constexpr Color Beige{ 245,245,220 };
    //  ���C�g�C�G���[
    //  #FFFFE0
    constexpr Color LightYellow{ 255,255,224 };
    //  ���C�g�S�[���f�����b�h�C�G���[
    //  #FAFAD2
    constexpr Color LightGoldenrodYellow{ 250,250,210 };
    //  �������V�t�H��
    //  #FFFACD
    constexpr Color LemonChiffon{ 255,250,205 };
    //  �E�B�[�g
    //  #F5DEB3
    constexpr Color Wheat{ 245,222,179 };
    //  �o�[���[�E�b�h
    //  #DEB887
    constexpr Color BurlyWood{ 222,184,135 };
    //  �^��
    //  #D2B48C
    constexpr Color Tan{ 210,180,140 };
    //  �J�[�L
    //  #F0E68C
    constexpr Color Khaki{ 240,230,140 };
    //  �C�G���[
    //  #FFFF00
    constexpr Color Yellow{ 255,255,0 };
    //  �S�[���h
    //  #FFD700
    constexpr Color Gold{ 255,215,0 };
    //  �I�����W
    //  #FFA500
    constexpr Color Orange{ 255,165,0 };
    //  �T���f�B�u���E��
    //  #F4A460
    constexpr Color SandyBrown{ 244,164,96 };
    //  �_�[�N�I�����W
    //  #FF8C00
    constexpr Color DarkOrange{ 255,140,0 };
    //  �S�[���f�����b�h
    //  #DAA520
    constexpr Color Goldenrod{ 218,165,32 };
    //  �y���[
    //  #CD853F
    constexpr Color Peru{ 205,133,63 };
    //  �_�[�N�S�[���f�����b�h
    //  #B8860B
    constexpr Color DarkGoldenrod{ 184,134,11 };
    //  �`���R���[�g
    //  #D2691E
    constexpr Color Chocolate{ 210,105,30 };
    //  �V�G�i
    //  #A0522D
    constexpr Color Sienna{ 160,82,45 };
    //  �T�h���u���E��
    //  #8B4513
    constexpr Color SaddleBrown{ 139,69,19 };
    //  �}���[��
    //  #800000
    constexpr Color Marron{ 128,0,0 };
    //  �_�[�N���b�h
    //  #8B0000
    constexpr Color DarkRed{ 139,0,0 };
    //  �u���E��
    //  #A52A2A
    constexpr Color Brown{ 165,42,42 };
    //  �t�@�C�A�u���b�N
    //  #B22222
    constexpr Color FireBrick{ 178,34,34 };
    //  �C���f�B�A�����b�h
    //  #CD5C5C
    constexpr Color IndianRed{ 205,92,92 };
    //  ���[�W�[�u���E��
    //  #BC8F8F
    constexpr Color RosyBrown{ 188,143,143 };
    //  �_�[�N�T�[����
    //  #E9967A
    constexpr Color DarkSalmon{ 233,150,122 };
    //  ���C�g�R�[����
    //  #F08080
    constexpr Color LightCoral{ 240,128,128 };
    //  �T�[����
    //  #FA8072
    constexpr Color Salmon{ 250,128,114 };
    //  ���C�g�T�[����
    //  #FFA07A
    constexpr Color LightSalmon{ 255,160,122 };
    //  �R�[����
    //  #FF7F50
    constexpr Color Coral{ 255,127,80 };
    //  �g�}�g
    //  #FF6347
    constexpr Color Tomato{ 255,99,71 };
    //  �I�����W���b�h
    //  #FF4500
    constexpr Color OrangeRed{ 255,69,0 };
    //  ���b�h
    //  #FF0000
    constexpr Color Red{ 255,0,0 };
    //  �N�����]��
    //  #DC143C
    constexpr Color Crimson{ 220,20,60 };
    //  �~�f�B�A���o�C�I���b�g���b�h
    //  #C71585
    constexpr Color MediumVioletRed{ 199,21,133 };
    //  �f�B�[�v�s���N
    //  #FF1493
    constexpr Color DeepPink{ 255,20,147 };
    //  �z�b�g�s���N
    //  #FF69B4
    constexpr Color HotPink{ 255,105,180 };
    //  �y�[���o�C�I���b�g���b�h
    //  #DB7093
    constexpr Color PaleVioletRed{ 219,112,147 };
    //  �s���N
    //  #FFC0CB
    constexpr Color Pink{ 255,192,203 };
    //  ���C�g�s���N
    //  #FFB6C1
    constexpr Color LightPink{ 255,182,193 };
    //  �V�X��
    //  #D8BFD8
    constexpr Color Thistle{ 216,191,216 };
    //  �}�[���^
    //  #FF00FF
    constexpr Color Magenta{ 255,0,255 };
    //  �t�N�V��
    //  #FF00FF
    constexpr Color Fuchsia{ 255,0,255 };
    //  �o�C�I���b�g
    //  #EE82EE
    constexpr Color Violet{ 238,130,238 };
    //  �v����
    //  #DDA0DD
    constexpr Color Plum{ 221,160,221 };
    //  �I�[�L�b�h
    //  #DA70D6
    constexpr Color Orchid{ 218,112,214 };
    //  �~�f�B�A���I�[�L�b�h
    //  #BA55D3
    constexpr Color MediumOrchid{ 186,85,211 };
    //  �_�[�N�I�[�L�b�h
    //  #9932CC
    constexpr Color DarkOrchid{ 153,50,204 };
    //  �_�[�N�o�C�I���b�g
    //  #9400D3
    constexpr Color DarkViolet{ 148,0,211 };
    //  �_�[�N�}�[���^
    //  #8B008B
    constexpr Color DarkMagenta{ 139,0,139 };
    //  �p�[�v��
    //  #800080
    constexpr Color Purple{ 128,0,128 };
    //  �C���f�B�S
    //  #4B0082
    constexpr Color Indigo{ 75,0,130 };
    //  �_�[�N�X���[�g�u���[
    //  #483D8B
    constexpr Color DarkSlateBlue{ 72,61,139 };
    //  �u���[�o�C�I���b�g
    //  #8A2BE2
    constexpr Color BlueViolet{ 138,43,226 };
    //  �~�f�B�A���p�[�v��
    //  #9370DB
    constexpr Color MediumPurple{ 147,112,219 };
    //  �X���[�g�u���[
    //  #6A5ACD
    constexpr Color SlateBlue{ 106,90,205 };
    //  �~�f�B�A���X���[�g�u���[
    //  #7B68EE
    constexpr Color MediumSlateBlue{ 123,104,238 };
  };
}

namespace std
{
  [[nodiscard]] inline string to_string(const vdl::ColorF& _Color)
  {
    return to_string(_Color.R) + "," + to_string(_Color.G) + "," + to_string(_Color.B) + "," + to_string(_Color.A);
  }

  [[nodiscard]] inline wstring to_wstring(const vdl::ColorF& _Color)
  {
    return to_wstring(_Color.R) + L"," + to_wstring(_Color.G) + L"," + to_wstring(_Color.B) + L"," + to_wstring(_Color.A);
  }

  template <class CharType>
  inline basic_ostream<CharType>& operator<<(basic_ostream<CharType>& _OStream, const vdl::ColorF& _Color)
  {
    return _OStream << _Color.R << CharType(',') << _Color.G << CharType(',') << _Color.B << CharType(',') << _Color.A;
  }

  template <class CharType>
  inline basic_istream<CharType>& operator>>(basic_istream<CharType>& _IStream, vdl::ColorF& _Color)
  {
    CharType Temp;

    return _IStream >> _Color.R >> Temp >> _Color.G >> Temp >> _Color.B >> Temp >> _Color.A;
  }
}
