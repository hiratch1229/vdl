#pragma once
#include "Fwd.hpp"
#include "Math.hpp"

#include <cmath>
#include <string>
#include <fstream>

//  関数がエラーを吐く為
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
    //  原色カラー140色
    //  https://www.colordic.org/

    //  ブラック
    //  #000000
    constexpr Color3 Black{ 0,0,0 };
    //  ディムグレイ
    //  #696969
    constexpr Color3 DimGray{ 105,105,105 };
    //  グレイ
    //  #808080
    constexpr Color3 Gray{ 128,128,128 };
    //  ダークグレイ
    //  #A9A9A9
    constexpr Color3 DarkGray{ 169,169,169 };
    //  シルバー
    //  #C0C0C0
    constexpr Color3 Silver{ 192,192,192 };
    //  ライトグレイ
    //  #D3D3D3
    constexpr Color3 LightGray{ 211,211,211 };
    //  ゲインズボロ
    //  #DCDCDC
    constexpr Color3 Gainsboro{ 220,220,220 };
    //  ホワイトスモーク
    //  #F5F5F5
    constexpr Color3 WhiteSmoke{ 245,245,245 };
    //  ホワイト
    //  #FFFFFF
    constexpr Color3 White{ 255,255,255 };
    //  スノウ
    //  #FFFAFA
    constexpr Color3 Snow{ 255,250,250 };
    //  ゴーストホワイト
    //  #F8F8FF
    constexpr Color3 GhostWhite{ 248,248,255 };
    //  フローラルホワイト
    //  #FFFAF0
    constexpr Color3 FloralWhite{ 255,250,240 };
    //  リネン
    //  #FAF0E6
    constexpr Color3 Linen{ 250,240,230 };
    //  アンティークホワイト
    //  #FAEBD7
    constexpr Color3 AntiqueWhite{ 250,235,215 };
    //  パパイヤホイップ
    //  #FFEFD5
    constexpr Color3 PapayaWhip{ 255,239,213 };
    //  ブランチドアーモンド
    //  #FFEBCD
    constexpr Color3 BlanchedAlmond{ 255,235,205 };
    //  ビスク
    //  #FFE4C4
    constexpr Color3 Bisque{ 255,228,196 };
    //  モカシン
    //  #FFE4B5
    constexpr Color3 Moccasin{ 255,75,181 };
    //  ナバホホワイト
    //  #FFDEAD
    constexpr Color3 NavajoWhite{ 255,222,173 };
    //  ピーチパフ
    //  #FFDAB9
    constexpr Color3 PeachPuff{ 255,218,185 };
    //  ミスティローズ
    //  #FFE4E1
    constexpr Color3 MistyRose{ 255,228,225 };
    //  ラベンダーブラシ
    //  #FFF0F5
    constexpr Color3 LavenderBlush{ 255,240,245 };
    //  シーシェル
    //  #FFF5EE
    constexpr Color3 SeaShell{ 255,245,238 };
    //  オールドレース
    //  #FDF5E6
    constexpr Color3 OldLace{ 253,245,230 };
    //  アイボリー
    //  #FFFFF0
    constexpr Color3 Ivory{ 255,255,240 };
    //  ハニーデュー
    //  #F0FFF0
    constexpr Color3 Honeydew{ 240,255,240 };
    //  ミントクリーム
    //  #F5FFFA
    constexpr Color3 MintCream{ 245,255,250 };
    //  アズール
    //  #F0FFFF
    constexpr Color3 Azure{ 240,255,255 };
    //  アリスブルー
    //  #F0F8FF
    constexpr Color3 AliceBlue{ 240,248,255 };
    //  ランベダー
    //  #E6E6FA
    constexpr Color3 Lavender{ 230,230,250 };
    //  ライトスティールブルー
    //  #B0C4DE
    constexpr Color3 LightSteelBlue{ 176,196,222 };
    //  ライトスレートグレイ
    //  #778899
    constexpr Color3 LightSlateGray{ 119,136,153 };
    //  スレートグレイ
    //  #708090
    constexpr Color3 SlateGray{ 112,128,144 };
    //  スティールブルー
    //  #4682B4
    constexpr Color3 SteelBlue{ 70,130,180 };
    //  ロイヤルブルー
    //  #4169E1
    constexpr Color3 RoyalBlue{ 65,105,225 };
    //  ミッドナイトブルー
    //  #191970
    constexpr Color3 MidnightBlue{ 25,25,112 };
    //  ネイビー
    //  #000080
    constexpr Color3 Navy{ 0,0,128 };
    //  ダークブルー
    //  #00008B
    constexpr Color3 DarkBlue{ 0,0,139 };
    //  ミディアムブルー
    //  #0000CD
    constexpr Color3 MediumBlue{ 0,0,205 };
    //  ブルー
    //  #0000FF
    constexpr Color3 Blue{ 0,0,255 };
    //  ドジャーブルー
    //  #1E90FF
    constexpr Color3 DodgerBlue{ 30,144,255 };
    //  コーンフラワーブルー
    //  #6495ED
    constexpr Color3 CornFlowerBlue{ 100,149,237 };
    //  ディープスカイブルー
    //  #00BFFF
    constexpr Color3 DeepSkyBlue{ 0,191,255 };
    //  ライトスカイブルー
    //  #87CEFA
    constexpr Color3 LightSkyBlue{ 135,206,250 };
    //  スカイブルー
    //  #87CEEB
    constexpr Color3 SkyBlue{ 135,206,235 };
    //  ライトブルー
    //  #ADD8E6
    constexpr Color3 LightBlue{ 173,216,230 };
    //  パウダーブルー
    //  #B0E0E6
    constexpr Color3 PowderBlue{ 176,224,230 };
    //  ペールターコイズ
    //  #AFEEEE
    constexpr Color3 PaleTurquoise{ 175,238,238 };
    //  ライトシアン
    //  #E0FFFF
    constexpr Color3 LightCyan{ 224,255,255 };
    //  シアン
    //  #00FFFF
    constexpr Color3 Cyan{ 0,255,255 };
    //  アクア
    //  #00FFFF
    constexpr Color3 Aqua{ 0,255,255 };
    //  ターコイズ
    //  #40E0D0
    constexpr Color3 Turquoise{ 64,224,208 };
    //  ミディアムターコイズ
    //  #48D1CC
    constexpr Color3 MediumTurquoise{ 72,209,204 };
    //  ダークターコイズ
    //  #00CED1
    constexpr Color3 DarkTurquoise{ 0,206,209 };
    //  ライトシーグリーン
    //  #20B2AA
    constexpr Color3 LightSeaGreen{ 32,178,170 };
    //  カデットブルー
    //  #5F9EA0
    constexpr Color3 CadetBlue{ 95,158,160 };
    //  ダークシアン
    //  #008B8B
    constexpr Color3 DarkCyan{ 0,139,139 };
    //  ティール
    //  #008080
    constexpr Color3 Teal{ 0,128,128 };
    //  ダークスレートグレイ
    //  #2F4F4F
    constexpr Color3 DarkSlateGray{ 47,79,79 };
    //  ダークグリーン
    //  #006400
    constexpr Color3 DarkGreen{ 0,100,0 };
    //  グリーン
    //  #008000
    constexpr Color3 Green{ 0,128,0 };
    //  フォレストグリーン
    //  #228B22
    constexpr Color3 ForestGreen{ 34,139,34 };
    //  シーグリーン
    //  #2E8B57
    constexpr Color3 SeaGreen{ 46,139,87 };
    //  ミディアムシーグリーン
    //  #3CB371
    constexpr Color3 MediumSeaGreen{ 60,179,113 };
    //  ミディアムアクアマリン
    //  #66CDAA
    constexpr Color3 MediumAquamarine{ 102,205,170 };
    //  ダークシーグリーン
    //  #8FBC8F
    constexpr Color3 DarkSeaGreen{ 143,188,143 };
    //  アクアマリン
    //  #7FFFD4
    constexpr Color3 Aquamarine{ 127,255,212 };
    //  ペールグリーン
    //  #98FB98
    constexpr Color3 PaleGreen{ 152,251,152 };
    //  ライトグリーン
    //  #90EE90
    constexpr Color3 LightGreen{ 144,238,144 };
    //  スプリンググリーン
    //  #00FF7F
    constexpr Color3 SpringGreen{ 0,255,127 };
    //  ミディアムスプリンググリーン
    //  #00FA9A
    constexpr Color3 MediumSpringGreen{ 0,250,154 };
    //  ローングリーン
    //  #7CFC00
    constexpr Color3 LawnGreen{ 124,252,0 };
    //  シャルトリューズ
    //  #7FFF00
    constexpr Color3 Chartreuse{ 127,255,0 };
    //  グリーンイエロー
    //  #ADFF2F
    constexpr Color3 GreenYellow{ 173,255,47 };
    //  ライム
    //  #00FF00
    constexpr Color3 Lime{ 0,255,0 };
    //  ライムグリーン
    //  #32CD32
    constexpr Color3 LimeGreen{ 50,205,50 };
    //  イエローグリーン
    //  #9ACD32
    constexpr Color3 YellowGreen{ 154,205,50 };
    //  ダークオリーブグリーン
    //  #556B2F
    constexpr Color3 DarkOliveGreen{ 85,107,47 };
    //  オリーブドラブ
    //  #6B8E23
    constexpr Color3 OliveDrab{ 107,142,35 };
    //  オリーブ
    //  #808000
    constexpr Color3 Olive{ 128,128,0 };
    //  ダークカーキ
    //  #BDB76B
    constexpr Color3 DarkKhaki{ 189,183,107 };
    //  ペールゴールデンロッド
    //  #EEE8AA
    constexpr Color3 PaleGoldenrod{ 238,232,170 };
    //  コーンシルク
    //  #FFF8DC
    constexpr Color3 CornSilk{ 255,248,220 };
    //  ベージュ
    //  #F5F5DC
    constexpr Color3 Beige{ 245,245,220 };
    //  ライトイエロー
    //  #FFFFE0
    constexpr Color3 LightYellow{ 255,255,224 };
    //  ライトゴールデンロッドイエロー
    //  #FAFAD2
    constexpr Color3 LightGoldenrodYellow{ 250,250,210 };
    //  レモンシフォン
    //  #FFFACD
    constexpr Color3 LemonChiffon{ 255,250,205 };
    //  ウィート
    //  #F5DEB3
    constexpr Color3 Wheat{ 245,222,179 };
    //  バーリーウッド
    //  #DEB887
    constexpr Color3 BurlyWood{ 222,184,135 };
    //  タン
    //  #D2B48C
    constexpr Color3 Tan{ 210,180,140 };
    //  カーキ
    //  #F0E68C
    constexpr Color3 Khaki{ 240,230,140 };
    //  イエロー
    //  #FFFF00
    constexpr Color3 Yellow{ 255,255,0 };
    //  ゴールド
    //  #FFD700
    constexpr Color3 Gold{ 255,215,0 };
    //  オレンジ
    //  #FFA500
    constexpr Color3 Orange{ 255,165,0 };
    //  サンディブラウン
    //  #F4A460
    constexpr Color3 SandyBrown{ 244,164,96 };
    //  ダークオレンジ
    //  #FF8C00
    constexpr Color3 DarkOrange{ 255,140,0 };
    //  ゴールデンロッド
    //  #DAA520
    constexpr Color3 Goldenrod{ 218,165,32 };
    //  ペルー
    //  #CD853F
    constexpr Color3 Peru{ 205,133,63 };
    //  ダークゴールデンロッド
    //  #B8860B
    constexpr Color3 DarkGoldenrod{ 184,134,11 };
    //  チョコレート
    //  #D2691E
    constexpr Color3 Chocolate{ 210,105,30 };
    //  シエナ
    //  #A0522D
    constexpr Color3 Sienna{ 160,82,45 };
    //  サドルブラウン
    //  #8B4513
    constexpr Color3 SaddleBrown{ 139,69,19 };
    //  マルーン
    //  #800000
    constexpr Color3 Marron{ 128,0,0 };
    //  ダークレッド
    //  #8B0000
    constexpr Color3 DarkRed{ 139,0,0 };
    //  ブラウン
    //  #A52A2A
    constexpr Color3 Brown{ 165,42,42 };
    //  ファイアブリック
    //  #B22222
    constexpr Color3 FireBrick{ 178,34,34 };
    //  インディアンレッド
    //  #CD5C5C
    constexpr Color3 IndianRed{ 205,92,92 };
    //  ロージーブラウン
    //  #BC8F8F
    constexpr Color3 RosyBrown{ 188,143,143 };
    //  ダークサーモン
    //  #E9967A
    constexpr Color3 DarkSalmon{ 233,150,122 };
    //  ライトコーラル
    //  #F08080
    constexpr Color3 LightCoral{ 240,128,128 };
    //  サーモン
    //  #FA8072
    constexpr Color3 Salmon{ 250,128,114 };
    //  ライトサーモン
    //  #FFA07A
    constexpr Color3 LightSalmon{ 255,160,122 };
    //  コーラル
    //  #FF7F50
    constexpr Color3 Coral{ 255,127,80 };
    //  トマト
    //  #FF6347
    constexpr Color3 Tomato{ 255,99,71 };
    //  オレンジレッド
    //  #FF4500
    constexpr Color3 OrangeRed{ 255,69,0 };
    //  レッド
    //  #FF0000
    constexpr Color3 Red{ 255,0,0 };
    //  クリムゾン
    //  #DC143C
    constexpr Color3 Crimson{ 220,20,60 };
    //  ミディアムバイオレットレッド
    //  #C71585
    constexpr Color3 MediumVioletRed{ 199,21,133 };
    //  ディープピンク
    //  #FF1493
    constexpr Color3 DeepPink{ 255,20,147 };
    //  ホットピンク
    //  #FF69B4
    constexpr Color3 HotPink{ 255,105,180 };
    //  ペールバイオレットレッド
    //  #DB7093
    constexpr Color3 PaleVioletRed{ 219,112,147 };
    //  ピンク
    //  #FFC0CB
    constexpr Color3 Pink{ 255,192,203 };
    //  ライトピンク
    //  #FFB6C1
    constexpr Color3 LightPink{ 255,182,193 };
    //  シスル
    //  #D8BFD8
    constexpr Color3 Thistle{ 216,191,216 };
    //  マゼンタ
    //  #FF00FF
    constexpr Color3 Magenta{ 255,0,255 };
    //  フクシャ
    //  #FF00FF
    constexpr Color3 Fuchsia{ 255,0,255 };
    //  バイオレット
    //  #EE82EE
    constexpr Color3 Violet{ 238,130,238 };
    //  プラム
    //  #DDA0DD
    constexpr Color3 Plum{ 221,160,221 };
    //  オーキッド
    //  #DA70D6
    constexpr Color3 Orchid{ 218,112,214 };
    //  ミディアムオーキッド
    //  #BA55D3
    constexpr Color3 MediumOrchid{ 186,85,211 };
    //  ダークオーキッド
    //  #9932CC
    constexpr Color3 DarkOrchid{ 153,50,204 };
    //  ダークバイオレット
    //  #9400D3
    constexpr Color3 DarkViolet{ 148,0,211 };
    //  ダークマゼンタ
    //  #8B008B
    constexpr Color3 DarkMagenta{ 139,0,139 };
    //  パープル
    //  #800080
    constexpr Color3 Purple{ 128,0,128 };
    //  インディゴ
    //  #4B0082
    constexpr Color3 Indigo{ 75,0,130 };
    //  ダークスレートブルー
    //  #483D8B
    constexpr Color3 DarkSlateBlue{ 72,61,139 };
    //  ブルーバイオレット
    //  #8A2BE2
    constexpr Color3 BlueViolet{ 138,43,226 };
    //  ミディアムパープル
    //  #9370DB
    constexpr Color3 MediumPurple{ 147,112,219 };
    //  スレートブルー
    //  #6A5ACD
    constexpr Color3 SlateBlue{ 106,90,205 };
    //  ミディアムスレートブルー
    //  #7B68EE
    constexpr Color3 MediumSlateBlue{ 123,104,238 };

    //-------------------------

    constexpr Color3 Diffuse{ 255, 255, 255 };

    constexpr Color3 NormalMap{ 128, 128, 255 };
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
