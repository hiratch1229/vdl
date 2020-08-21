namespace vdl::GUI
{
  namespace Detail
  {
    // A primary data type
    enum class DataType
    {
      eS8,      // signed char / char (with sensible compilers)
      eU8,      // unsigned char
      eS16,     // short
      eU16,     // unsigned short
      eS32,     // int
      eU32,     // unsigned int
      eS64,     // long long / __int64
      eU64,     // unsigned long long / unsigned __int64
      eFloat,   // float
      eDouble,  // double
    };

    template<class Type>
    inline constexpr bool CheckType()
    {
      return std::is_fundamental<Type>::value || std::is_same<Radian, Type>::value || std::is_same<Degree, Type>::value;
    }

    template<class Type>
    inline constexpr DataType GetDataType()
    {
      static_assert(CheckType<Type>());

      if constexpr (std::is_same<int8_t, Type>::value)
      {
        return DataType::eS8;
      }
      else if constexpr (std::is_same<uint8_t, Type>::value)
      {
        return DataType::eU8;
      }
      else if constexpr (std::is_same<int16_t, Type>::value)
      {
        return DataType::eS16;
      }
      else if constexpr (std::is_same<uint16_t, Type>::value)
      {
        return DataType::eU16;
      }
      else if constexpr (std::is_same<int32_t, Type>::value)
      {
        return DataType::eS32;
      }
      else if constexpr (std::is_same<uint32_t, Type>::value)
      {
        return DataType::eU32;
      }
      else if constexpr (std::is_same<int64_t, Type>::value)
      {
        return DataType::eS64;
      }
      else if constexpr (std::is_same<uint64_t, Type>::value)
      {
        return DataType::eU64;
      }
      else if constexpr (std::is_same<float, Type>::value || std::is_same<Radian, Type>::value || std::is_same<Degree, Type>::value)
      {
        return DataType::eFloat;
      }
      else if constexpr (std::is_same<double, Type>::value)
      {
        return DataType::eDouble;
      }
    }

    template<class Type>
    inline constexpr const char* GetDefaultFormat()
    {
      static_assert(CheckType<Type>());

      if constexpr (std::is_same<int8_t, Type>::value || std::is_same<int16_t, Type>::value || std::is_same<int32_t, Type>::value)
      {
        return "%d";
      }
      else if constexpr (std::is_same<uint8_t, Type>::value || std::is_same<uint16_t, Type>::value || std::is_same<uint32_t, Type>::value)
      {
        return "%u";
      }
      else if constexpr (std::is_same<int64_t, Type>::value)
      {
        return "%lld";
      }
      else if constexpr (std::is_same<uint64_t, Type>::value)
      {
        return "%llu";
      }
      else if constexpr (std::is_same<float, Type>::value || std::is_same<double, Type>::value || std::is_same<Radian, Type>::value || std::is_same<Degree, Type>::value)
      {
        return "%f";
      }
    }

    template<class Type>
    inline constexpr auto GetStep()
    {
      if constexpr (std::is_integral<Type>::value)
      {
        return static_cast<Type>(1);
      }
      else
      {
        return static_cast<Type>(0);
      }
    }

    template<class Type>
    inline constexpr auto GetStepFast()
    {
      if constexpr (std::is_integral<Type>::value)
      {
        return static_cast<Type>(100);
      }
      else
      {
        return static_cast<Type>(0);
      }
    }

    bool DragScalar(const char* _Label, DataType _Type, void* _pData, float _Speed, const void* _pMin, const void* _pMax, const char* _Format, const SliderFlags& _Flags);
    bool DragScalarN(const char* _Label, DataType _Type, void* _pData, int _Components, float _Speed, const void* _pMin, const void* _pMax, const char* _Format, const SliderFlags& _Flags);

    bool SliderScalar(const char* _Label, DataType _Type, void* _pData, const void* _pMin, const void* _pMax, const char* _Format, const SliderFlags& _Flags);
    bool SliderScalarN(const char* _Label, DataType _Type, void* _pData, int _Components, const void* _pMin, const void* _pMax, const char* _Format, const SliderFlags& _Flags);

    bool InputScalar(const char* _Label, DataType _Type, void* _pData, const void* _pStep, const void* _pStepFast, const char* _Format, const InputTextFlags& _Flags);
    bool InputScalarN(const char* _Label, DataType _Type, void* _pData, int _Components, const void* _pStep, const void* _pStepFast, const char* _Format, const InputTextFlags& _Flags);
  }

  //--------------------------------------------------

  template<class Type>
  inline bool Drag(const char* _Label, Type* _pData, float _Speed = 1.0f, Type _Min = 0, Type _Max = 0, const char* _Format = Detail::GetDefaultFormat<Type>(), const SliderFlags& _Flags = SliderFlag::eNone)
  {
    return DragScalar(_Label, Detail::GetDataType<Type>(), _pData, _Speed, &_Min, &_Max, _Format, _Flags);
  }
  template<class Type>
  inline bool Drag(const char* _Label, Type2<Type>* _pData, float _Speed = 1.0f, Type _Min = 0, Type _Max = 0, const char* _Format = Detail::GetDefaultFormat<Type>(), const SliderFlags& _Flags = SliderFlag::eNone)
  {
    return DragScalarN(_Label, Detail::GetDataType<Type>(), _pData, 2, _Speed, &_Min, &_Max, _Format, _Flags);
  }
  template<class Type>
  inline bool Drag(const char* _Label, Type3<Type>* _pData, float _Speed = 1.0f, Type _Min = 0, Type _Max = 0, const char* _Format = Detail::GetDefaultFormat<Type>(), const SliderFlags& _Flags = SliderFlag::eNone)
  {
    return DragScalarN(_Label, Detail::GetDataType<Type>(), _pData, 3, _Speed, &_Min, &_Max, _Format, _Flags);
  }
  template<class Type>
  inline bool Drag(const char* _Label, Type4<Type>* _pData, float _Speed = 1.0f, Type _Min = 0, Type _Max = 0, const char* _Format = Detail::GetDefaultFormat<Type>(), const SliderFlags& _Flags = SliderFlag::eNone)
  {
    return DragScalarN(_Label, Detail::GetDataType<Type>(), _pData, 4, _Speed, &_Min, &_Max, _Format, _Flags);
  }

  //--------------------------------------------------

  template<class Type>
  inline bool Slider(const char* _Label, Type* _pData, Type _Min, Type _Max, const char* _Format = Detail::GetDefaultFormat<Type>(), const SliderFlags& _Flags = SliderFlag::eNone)
  {
    return SliderScalar(_Label, Detail::GetDataType<Type>(), _pData, &_Min, &_Max, _Format, _Flags);
  }
  template<class Type>
  inline bool Slider(const char* _Label, Type2<Type>* _pData, Type _Min, Type _Max, const char* _Format = Detail::GetDefaultFormat<Type>(), const SliderFlags& _Flags = SliderFlag::eNone)
  {
    return SliderScalarN(_Label, Detail::GetDataType<Type>(), _pData, 2, &_Min, &_Max, _Format, _Flags);
  }
  template<class Type>
  inline bool Slider(const char* _Label, Type3<Type>* _pData, Type _Min, Type _Max, const char* _Format = Detail::GetDefaultFormat<Type>(), const SliderFlags& _Flags = SliderFlag::eNone)
  {
    return SliderScalarN(_Label, Detail::GetDataType<Type>(), _pData, 3, &_Min, &_Max, _Format, _Flags);
  }
  template<class Type>
  inline bool Slider(const char* _Label, Type4<Type>* _pData, Type _Min, Type _Max, const char* _Format = Detail::GetDefaultFormat<Type>(), const SliderFlags& _Flags = SliderFlag::eNone)
  {
    return SliderScalarN(_Label, Detail::GetDataType<Type>(), _pData, 4, &_Min, &_Max, _Format, _Flags);
  }

  //--------------------------------------------------

  template<class Type>
  inline bool Input(const char* _Label, Type* _pData, Type _Step = Detail::GetStep<Type>(), Type _StepFast = Detail::GetStepFast<Type>(), const char* _Format = Detail::GetDefaultFormat<Type>(), const InputTextFlags& _Flags = InputTextFlag::eNone)
  {
    return InputScalar(_Label, Detail::GetDataType<Type>(), _pData, (_Step > static_cast<Type>(0) ? &_Step : nullptr), (_StepFast > static_cast<Type>(0) ? &_StepFast : nullptr), _Format, _Flags);
  }
  template<class Type>
  inline bool Input(const char* _Label, Type2<Type>* _pData, const char* _Format = Detail::GetDefaultFormat<Type>(), const InputTextFlags& _Flags = InputTextFlag::eNone)
  {
    return InputScalarN(_Label, Detail::GetDataType<Type>(), _pData, 2, nullptr, nullptr, _Format, _Flags);
  }
  template<class Type>
  inline bool Input(const char* _Label, Type3<Type>* _pData, const char* _Format = Detail::GetDefaultFormat<Type>(), const InputTextFlags& _Flags = InputTextFlag::eNone)
  {
    return InputScalarN(_Label, Detail::GetDataType<Type>(), _pData, 3, nullptr, nullptr, _Format, _Flags);
  }
  template<class Type>
  inline bool Input(const char* _Label, Type4<Type>* _pData, const char* _Format = Detail::GetDefaultFormat<Type>(), const InputTextFlags& _Flags = InputTextFlag::eNone)
  {
    return InputScalarN(_Label, Detail::GetDataType<Type>(), _pData, 4, nullptr, nullptr, _Format, _Flags);
  }
}
