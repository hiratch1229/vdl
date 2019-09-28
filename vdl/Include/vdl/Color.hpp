#pragma once
#include "Fwd.hpp"
#include "Types.hpp"

#include <cmath>
#include <string>
#include <fstream>

namespace vdl
{
  struct Color
  {
#pragma warning(disable:4201)
    union
    {
      struct
      {
        //  赤
        uint8_t Red;
        //  緑
        uint8_t Green;
        //  青
        uint8_t Blue;
        //  不透明度
        uint8_t Alpha;
      };
      uint32_t Bit;
    };
#pragma warning(default:4201)
  public:
    Color() = default;

    constexpr Color(uint8_t _Red, uint8_t _Green, uint8_t _Blue, uint8_t _Alpha = 255u)noexcept
      : Red(_Red), Green(_Green), Blue(_Blue), Alpha(_Alpha) {}

    constexpr Color(uint8_t _RGB, uint8_t _Alpha = 255u)noexcept
      : Red(_RGB), Green(_RGB), Blue(_RGB), Alpha(_Alpha) {}

    constexpr Color(const Color& _RGB, uint8_t _Alpha)noexcept
      : Red(_RGB.Red), Green(_RGB.Green), Blue(_RGB.Blue), Alpha(_Alpha) {}

    constexpr Color(const ColorF& _Color)noexcept;
  public:
    [[nodiscard]] constexpr bool operator==(const Color& _Color)const noexcept { return Bit == _Color.Bit; }

    [[nodiscard]] constexpr bool operator!=(const Color& _Color)const noexcept { return Bit != _Color.Bit; }
  };

  struct ColorF
  {
    //  赤
    float Red;
    //  緑
    float Green;
    //  青
    float Blue;
    //  不透明度
    float Alpha;
  public:
    ColorF() = default;

    constexpr ColorF(float _Red, float _Green, float _Blue, float _Alpha = 1.0f)noexcept
      : Red(_Red), Green(_Green), Blue(_Blue), Alpha(_Alpha) {}

    constexpr ColorF(float _RGB, float _Alpha = 1.0f)noexcept
      : Red(_RGB), Green(_RGB), Blue(_RGB), Alpha(_Alpha) {}

    constexpr ColorF(const ColorF& _RGB, float _Alpha)noexcept
      : Red(_RGB.Red), Green(_RGB.Green), Blue(_RGB.Blue), Alpha(_Alpha) {}

    constexpr ColorF(const Color& _Color)noexcept;
  public:
    [[nodiscard]] constexpr bool operator ==(const ColorF& _Color)const noexcept { return Red == _Color.Red && Green == _Color.Green && Blue == _Color.Blue && Alpha == _Color.Alpha; }

    [[nodiscard]] constexpr bool operator !=(const ColorF& _Color)const noexcept { return Red != _Color.Red || Green != _Color.Green || Blue != _Color.Blue || Alpha != _Color.Alpha; }
  };

  inline constexpr Color::Color(const ColorF& _Color)noexcept
    : Red(static_cast<uint8_t>(_Color.Red * 255.0f)), Green(static_cast<uint8_t>(_Color.Green * 255.0f)), Blue(static_cast<uint8_t>(_Color.Blue * 255.0f)), Alpha(static_cast<uint8_t>(_Color.Alpha * 255.0f)) {}

  inline constexpr ColorF::ColorF(const Color& _Color)noexcept
    : Red(_Color.Red / 255.0f), Green(_Color.Green / 255.0f), Blue(_Color.Blue / 255.0f), Alpha(_Color.Alpha / 255.0f) {}

  //  原色カラー140色
  namespace Palette
  {
    //  色見本
    //  https://www.colordic.org/

    //  ブラック
    //  #000000
    constexpr Color Black{ 0,0,0 };
    //  ディムグレイ
    //  #696969
    constexpr Color DimGray{ 105,105,105 };
    //  グレイ
    //  #808080
    constexpr Color Gray{ 128,128,128 };
    //  ダークグレイ
    //  #A9A9A9
    constexpr Color DarkGray{ 169,169,169 };
    //  シルバー
    //  #C0C0C0
    constexpr Color Silver{ 192,192,192 };
    //  ライトグレイ
    //  #D3D3D3
    constexpr Color LightGray{ 211,211,211 };
    //  ゲインズボロ
    //  #DCDCDC
    constexpr Color Gainsboro{ 220,220,220 };
    //  ホワイトスモーク
    //  #F5F5F5
    constexpr Color WhiteSmoke{ 245,245,245 };
    //  ホワイト
    //  #FFFFFF
    constexpr Color White{ 255,255,255 };
    //  スノウ
    //  #FFFAFA
    constexpr Color Snow{ 255,250,250 };
    //  ゴーストホワイト
    //  #F8F8FF
    constexpr Color GhostWhite{ 248,248,255 };
    //  フローラルホワイト
    //  #FFFAF0
    constexpr Color FloralWhite{ 255,250,240 };
    //  リネン
    //  #FAF0E6
    constexpr Color Linen{ 250,240,230 };
    //  アンティークホワイト
    //  #FAEBD7
    constexpr Color AntiqueWhite{ 250,235,215 };
    //  パパイヤホイップ
    //  #FFEFD5
    constexpr Color PapayaWhip{ 255,239,213 };
    //  ブランチドアーモンド
    //  #FFEBCD
    constexpr Color BlanchedAlmond{ 255,235,205 };
    //  ビスク
    //  #FFE4C4
    constexpr Color Bisque{ 255,228,196 };
    //  モカシン
    //  #FFE4B5
    constexpr Color Moccasin{ 255,75,181 };
    //  ナバホホワイト
    //  #FFDEAD
    constexpr Color NavajoWhite{ 255,222,173 };
    //  ピーチパフ
    //  #FFDAB9
    constexpr Color PeachPuff{ 255,218,185 };
    //  ミスティローズ
    //  #FFE4E1
    constexpr Color MistyRose{ 255,228,225 };
    //  ラベンダーブラシ
    //  #FFF0F5
    constexpr Color LavenderBlush{ 255,240,245 };
    //  シーシェル
    //  #FFF5EE
    constexpr Color SeaShell{ 255,245,238 };
    //  オールドレース
    //  #FDF5E6
    constexpr Color OldLace{ 253,245,230 };
    //  アイボリー
    //  #FFFFF0
    constexpr Color Ivory{ 255,255,240 };
    //  ハニーデュー
    //  #F0FFF0
    constexpr Color Honeydew{ 240,255,240 };
    //  ミントクリーム
    //  #F5FFFA
    constexpr Color MintCream{ 245,255,250 };
    //  アズール
    //  #F0FFFF
    constexpr Color Azure{ 240,255,255 };
    //  アリスブルー
    //  #F0F8FF
    constexpr Color AliceBlue{ 240,248,255 };
    //  ランベダー
    //  #E6E6FA
    constexpr Color Lavender{ 230,230,250 };
    //  ライトスティールブルー
    //  #B0C4DE
    constexpr Color LightSteelBlue{ 176,196,222 };
    //  ライトスレートグレイ
    //  #778899
    constexpr Color LightSlateGray{ 119,136,153 };
    //  スレートグレイ
    //  #708090
    constexpr Color SlateGray{ 112,128,144 };
    //  スティールブルー
    //  #4682B4
    constexpr Color SteelBlue{ 70,130,180 };
    //  ロイヤルブルー
    //  #4169E1
    constexpr Color RoyalBlue{ 65,105,225 };
    //  ミッドナイトブルー
    //  #191970
    constexpr Color MidnightBlue{ 25,25,112 };
    //  ネイビー
    //  #000080
    constexpr Color Navy{ 0,0,128 };
    //  ダークブルー
    //  #00008B
    constexpr Color DarkBlue{ 0,0,139 };
    //  ミディアムブルー
    //  #0000CD
    constexpr Color MediumBlue{ 0,0,205 };
    //  ブルー
    //  #0000FF
    constexpr Color Blue{ 0,0,255 };
    //  ドジャーブルー
    //  #1E90FF
    constexpr Color DodgerBlue{ 30,144,255 };
    //  コーンフラワーブルー
    //  #6495ED
    constexpr Color CornFlowerBlue{ 100,149,237 };
    //  ディープスカイブルー
    //  #00BFFF
    constexpr Color DeepSkyBlue{ 0,191,255 };
    //  ライトスカイブルー
    //  #87CEFA
    constexpr Color LightSkyBlue{ 135,206,250 };
    //  スカイブルー
    //  #87CEEB
    constexpr Color SkyBlue{ 135,206,235 };
    //  ライトブルー
    //  #ADD8E6
    constexpr Color LightBlue{ 173,216,230 };
    //  パウダーブルー
    //  #B0E0E6
    constexpr Color PowderBlue{ 176,224,230 };
    //  ペールターコイズ
    //  #AFEEEE
    constexpr Color PaleTurquoise{ 175,238,238 };
    //  ライトシアン
    //  #E0FFFF
    constexpr Color LightCyan{ 224,255,255 };
    //  シアン
    //  #00FFFF
    constexpr Color Cyan{ 0,255,255 };
    //  アクア
    //  #00FFFF
    constexpr Color Aqua{ 0,255,255 };
    //  ターコイズ
    //  #40E0D0
    constexpr Color Turquoise{ 64,224,208 };
    //  ミディアムターコイズ
    //  #48D1CC
    constexpr Color MediumTurquoise{ 72,209,204 };
    //  ダークターコイズ
    //  #00CED1
    constexpr Color DarkTurquoise{ 0,206,209 };
    //  ライトシーグリーン
    //  #20B2AA
    constexpr Color LightSeaGreen{ 32,178,170 };
    //  カデットブルー
    //  #5F9EA0
    constexpr Color CadetBlue{ 95,158,160 };
    //  ダークシアン
    //  #008B8B
    constexpr Color DarkCyan{ 0,139,139 };
    //  ティール
    //  #008080
    constexpr Color Teal{ 0,128,128 };
    //  ダークスレートグレイ
    //  #2F4F4F
    constexpr Color DarkSlateGray{ 47,79,79 };
    //  ダークグリーン
    //  #006400
    constexpr Color DarkGreen{ 0,100,0 };
    //  グリーン
    //  #008000
    constexpr Color Green{ 0,128,0 };
    //  フォレストグリーン
    //  #228B22
    constexpr Color ForestGreen{ 34,139,34 };
    //  シーグリーン
    //  #2E8B57
    constexpr Color SeaGreen{ 46,139,87 };
    //  ミディアムシーグリーン
    //  #3CB371
    constexpr Color MediumSeaGreen{ 60,179,113 };
    //  ミディアムアクアマリン
    //  #66CDAA
    constexpr Color MediumAquamarine{ 102,205,170 };
    //  ダークシーグリーン
    //  #8FBC8F
    constexpr Color DarkSeaGreen{ 143,188,143 };
    //  アクアマリン
    //  #7FFFD4
    constexpr Color Aquamarine{ 127,255,212 };
    //  ペールグリーン
    //  #98FB98
    constexpr Color PaleGreen{ 152,251,152 };
    //  ライトグリーン
    //  #90EE90
    constexpr Color LightGreen{ 144,238,144 };
    //  スプリンググリーン
    //  #00FF7F
    constexpr Color SpringGreen{ 0,255,127 };
    //  ミディアムスプリンググリーン
    //  #00FA9A
    constexpr Color MediumSpringGreen{ 0,250,154 };
    //  ローングリーン
    //  #7CFC00
    constexpr Color LawnGreen{ 124,252,0 };
    //  シャルトリューズ
    //  #7FFF00
    constexpr Color Chartreuse{ 127,255,0 };
    //  グリーンイエロー
    //  #ADFF2F
    constexpr Color GreenYellow{ 173,255,47 };
    //  ライム
    //  #00FF00
    constexpr Color Lime{ 0,255,0 };
    //  ライムグリーン
    //  #32CD32
    constexpr Color LimeGreen{ 50,205,50 };
    //  イエローグリーン
    //  #9ACD32
    constexpr Color YellowGreen{ 154,205,50 };
    //  ダークオリーブグリーン
    //  #556B2F
    constexpr Color DarkOliveGreen{ 85,107,47 };
    //  オリーブドラブ
    //  #6B8E23
    constexpr Color OliveDrab{ 107,142,35 };
    //  オリーブ
    //  #808000
    constexpr Color Olive{ 128,128,0 };
    //  ダークカーキ
    //  #BDB76B
    constexpr Color DarkKhaki{ 189,183,107 };
    //  ペールゴールデンロッド
    //  #EEE8AA
    constexpr Color PaleGoldenrod{ 238,232,170 };
    //  コーンシルク
    //  #FFF8DC
    constexpr Color CornSilk{ 255,248,220 };
    //  ベージュ
    //  #F5F5DC
    constexpr Color Beige{ 245,245,220 };
    //  ライトイエロー
    //  #FFFFE0
    constexpr Color LightYellow{ 255,255,224 };
    //  ライトゴールデンロッドイエロー
    //  #FAFAD2
    constexpr Color LightGoldenrodYellow{ 250,250,210 };
    //  レモンシフォン
    //  #FFFACD
    constexpr Color LemonChiffon{ 255,250,205 };
    //  ウィート
    //  #F5DEB3
    constexpr Color Wheat{ 245,222,179 };
    //  バーリーウッド
    //  #DEB887
    constexpr Color BurlyWood{ 222,184,135 };
    //  タン
    //  #D2B48C
    constexpr Color Tan{ 210,180,140 };
    //  カーキ
    //  #F0E68C
    constexpr Color Khaki{ 240,230,140 };
    //  イエロー
    //  #FFFF00
    constexpr Color Yellow{ 255,255,0 };
    //  ゴールド
    //  #FFD700
    constexpr Color Gold{ 255,215,0 };
    //  オレンジ
    //  #FFA500
    constexpr Color Orange{ 255,165,0 };
    //  サンディブラウン
    //  #F4A460
    constexpr Color SandyBrown{ 244,164,96 };
    //  ダークオレンジ
    //  #FF8C00
    constexpr Color DarkOrange{ 255,140,0 };
    //  ゴールデンロッド
    //  #DAA520
    constexpr Color Goldenrod{ 218,165,32 };
    //  ペルー
    //  #CD853F
    constexpr Color Peru{ 205,133,63 };
    //  ダークゴールデンロッド
    //  #B8860B
    constexpr Color DarkGoldenrod{ 184,134,11 };
    //  チョコレート
    //  #D2691E
    constexpr Color Chocolate{ 210,105,30 };
    //  シエナ
    //  #A0522D
    constexpr Color Sienna{ 160,82,45 };
    //  サドルブラウン
    //  #8B4513
    constexpr Color SaddleBrown{ 139,69,19 };
    //  マルーン
    //  #800000
    constexpr Color Marron{ 128,0,0 };
    //  ダークレッド
    //  #8B0000
    constexpr Color DarkRed{ 139,0,0 };
    //  ブラウン
    //  #A52A2A
    constexpr Color Brown{ 165,42,42 };
    //  ファイアブリック
    //  #B22222
    constexpr Color FireBrick{ 178,34,34 };
    //  インディアンレッド
    //  #CD5C5C
    constexpr Color IndianRed{ 205,92,92 };
    //  ロージーブラウン
    //  #BC8F8F
    constexpr Color RosyBrown{ 188,143,143 };
    //  ダークサーモン
    //  #E9967A
    constexpr Color DarkSalmon{ 233,150,122 };
    //  ライトコーラル
    //  #F08080
    constexpr Color LightCoral{ 240,128,128 };
    //  サーモン
    //  #FA8072
    constexpr Color Salmon{ 250,128,114 };
    //  ライトサーモン
    //  #FFA07A
    constexpr Color LightSalmon{ 255,160,122 };
    //  コーラル
    //  #FF7F50
    constexpr Color Coral{ 255,127,80 };
    //  トマト
    //  #FF6347
    constexpr Color Tomato{ 255,99,71 };
    //  オレンジレッド
    //  #FF4500
    constexpr Color OrangeRed{ 255,69,0 };
    //  レッド
    //  #FF0000
    constexpr Color Red{ 255,0,0 };
    //  クリムゾン
    //  #DC143C
    constexpr Color Crimson{ 220,20,60 };
    //  ミディアムバイオレットレッド
    //  #C71585
    constexpr Color MediumVioletRed{ 199,21,133 };
    //  ディープピンク
    //  #FF1493
    constexpr Color DeepPink{ 255,20,147 };
    //  ホットピンク
    //  #FF69B4
    constexpr Color HotPink{ 255,105,180 };
    //  ペールバイオレットレッド
    //  #DB7093
    constexpr Color PaleVioletRed{ 219,112,147 };
    //  ピンク
    //  #FFC0CB
    constexpr Color Pink{ 255,192,203 };
    //  ライトピンク
    //  #FFB6C1
    constexpr Color LightPink{ 255,182,193 };
    //  シスル
    //  #D8BFD8
    constexpr Color Thistle{ 216,191,216 };
    //  マゼンタ
    //  #FF00FF
    constexpr Color Magenta{ 255,0,255 };
    //  フクシャ
    //  #FF00FF
    constexpr Color Fuchsia{ 255,0,255 };
    //  バイオレット
    //  #EE82EE
    constexpr Color Violet{ 238,130,238 };
    //  プラム
    //  #DDA0DD
    constexpr Color Plum{ 221,160,221 };
    //  オーキッド
    //  #DA70D6
    constexpr Color Orchid{ 218,112,214 };
    //  ミディアムオーキッド
    //  #BA55D3
    constexpr Color MediumOrchid{ 186,85,211 };
    //  ダークオーキッド
    //  #9932CC
    constexpr Color DarkOrchid{ 153,50,204 };
    //  ダークバイオレット
    //  #9400D3
    constexpr Color DarkViolet{ 148,0,211 };
    //  ダークマゼンタ
    //  #8B008B
    constexpr Color DarkMagenta{ 139,0,139 };
    //  パープル
    //  #800080
    constexpr Color Purple{ 128,0,128 };
    //  インディゴ
    //  #4B0082
    constexpr Color Indigo{ 75,0,130 };
    //  ダークスレートブルー
    //  #483D8B
    constexpr Color DarkSlateBlue{ 72,61,139 };
    //  ブルーバイオレット
    //  #8A2BE2
    constexpr Color BlueViolet{ 138,43,226 };
    //  ミディアムパープル
    //  #9370DB
    constexpr Color MediumPurple{ 147,112,219 };
    //  スレートブルー
    //  #6A5ACD
    constexpr Color SlateBlue{ 106,90,205 };
    //  ミディアムスレートブルー
    //  #7B68EE
    constexpr Color MediumSlateBlue{ 123,104,238 };
  };
}

namespace std
{
  [[nodiscard]] inline string to_string(const vdl::ColorF& _Color)
  {
    return to_string(_Color.Red) + "," + to_string(_Color.Green) + "," + to_string(_Color.Blue) + "," + to_string(_Color.Alpha);
  }

  [[nodiscard]] inline wstring to_wstring(const vdl::ColorF& _Color)
  {
    return to_wstring(_Color.Red) + L"," + to_wstring(_Color.Green) + L"," + to_wstring(_Color.Blue) + L"," + to_wstring(_Color.Alpha);
  }

  template <class CharType>
  inline basic_ostream<CharType>& operator<<(basic_ostream<CharType>& _OStream, const vdl::ColorF& _Color)
  {
    return _OStream << _Color.Red << static_cast<CharType>(',') << _Color.Green << static_cast<CharType>(',') << _Color.Blue << static_cast<CharType>(',') << _Color.Alpha;
  }

  template <class CharType>
  inline basic_istream<CharType>& operator>>(basic_istream<CharType>& _IStream, vdl::ColorF& _Color)
  {
    CharType Temp;

    return _IStream >> _Color.Red >> Temp >> _Color.Green >> Temp >> _Color.Blue >> Temp >> _Color.Alpha;
  }
}
