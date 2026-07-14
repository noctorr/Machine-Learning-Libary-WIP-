#pragma once

#include <bitset>
#include <bit>
#include <cmath>
#include <cstdint>
#include <type_traits>
#include <iostream>
#include <limits>
#include <memory>
#include <optional>

namespace utl
{
inline std::optional<size_t> __check_exp_bits ( size_t total_bits )
{
    switch ( total_bits )
    {
        case 16uz: return 5uz;
        case 32uz: return 8uz;
        case 64uz: return 11uz;
        case 128uz: return 15uz;
        default: return std::nullopt;
    }
}
}
template < typename __FLOAT_TYPE >
requires std::is_floating_point_v<__FLOAT_TYPE>
struct FLOAT_VALUE final
{
    std::bitset<sizeof(__FLOAT_TYPE) * 8uz> m_bits;
    __FLOAT_TYPE m_type_holder;
};

template < typename __FLOAT_TYPE >
requires std::is_floating_point_v<__FLOAT_TYPE>
class IEE_754_FLOAT final
{
    private:
    const size_t m_size { sizeof(__FLOAT_TYPE) * 8uz };
    FLOAT_VALUE<__FLOAT_TYPE> m_value;

    public:
    IEE_754_FLOAT(
        __FLOAT_TYPE _input_float
    )
    {
        if ( _input_float >= static_cast<__FLOAT_TYPE>(UINT64_MAX) )
        {
            m_value.m_bits = std::bitset<sizeof(__FLOAT_TYPE) * 8uz>(_input_float);
        } else
        {
            m_value.m_bits = std::bitset<sizeof(__FLOAT_TYPE) * 8uz>(std::bit_cast<uint64_t, __FLOAT_TYPE>(_input_float));
        }
    }

    auto operator+ ( IEE_754_FLOAT& _other_float ) noexcept(true)
    {
        using __SECOND_FLOAT_TYPE = decltype( _other_float.m_value.m_type_holder );

        if ( std::is_same_v<__FLOAT_TYPE, __SECOND_FLOAT_TYPE> )
        {
            std::cout << m_size << std::endl;
            {
                std::unique_ptr<std::bitset<sizeof(__FLOAT_TYPE) * 8uz>> _reverse_bits { std::make_unique<std::bitset<sizeof(__FLOAT_TYPE) * 8uz>>(m_value.m_bits) };
                std::unique_ptr<std::bitset<sizeof(__FLOAT_TYPE) * 8uz>> _second_reverse_bits { std::make_unique<std::bitset<sizeof(__FLOAT_TYPE) * 8uz>>(_other_float.m_value.m_bits) };

                for ( size_t i { 0uz }; i < m_size; i++ )
                {
                    (*_reverse_bits)[i] = m_value.m_bits[m_size-1-i];
                    (*_second_reverse_bits)[i] = _other_float.m_value.m_bits[m_size-1-i];
                }

                m_value.m_bits = *_reverse_bits;
                _other_float.m_value.m_bits = *_second_reverse_bits;
                std::cout << m_value.m_bits << std::endl;
                std::cout << _other_float.m_value.m_bits << std::endl;
            }

            __FLOAT_TYPE this_Mantissa { static_cast<__FLOAT_TYPE>(0) };
            __FLOAT_TYPE this_Exponent { static_cast<__FLOAT_TYPE>(0) };

            std::bitset<sizeof(__FLOAT_TYPE) * 8uz> this_bits = m_value.m_bits;

            const size_t exp_bit_size = utl::__check_exp_bits(m_size);
            for ( size_t i { 1uz }; i < exp_bit_size + 1uz; i++ )
            {
                if ( this_bits[i] == 1 )
                {
                    this_Exponent += static_cast<__FLOAT_TYPE>(std::pow(2, i));
                }
            }

            for ( size_t i { exp_bit_size + 1uz }; i < m_size; i++ )
            {
                if ( i == exp_bit_size + 1uz && this_bits[i] == 1 )
                {
                    this_Mantissa += static_cast<__FLOAT_TYPE>(1);
                } else if ( this_bits[i] == 1 )
                {
                    std::cout << i << "lol" << std::endl;
                    this_Mantissa += static_cast<__FLOAT_TYPE>(1) / static_cast<__FLOAT_TYPE>(std::pow(2, i-(exp_bit_size+1uz)));
                }
            }

            __FLOAT_TYPE other_Mantissa { static_cast<__FLOAT_TYPE>(0) };
            __FLOAT_TYPE other_Exponent { static_cast<__FLOAT_TYPE>(0) };

            std::bitset<sizeof(__FLOAT_TYPE) * 8uz> other_bits = _other_float.m_value.m_bits;

            for ( size_t i { 1uz }; i < exp_bit_size + 1uz; i++ )
            {
                if ( other_bits[i] == 1 )
                {
                    std::cout << "hi " << std::pow(2, i) << " " << i << std::endl;
                    other_Exponent += static_cast<__FLOAT_TYPE>(std::pow(2, i));
                }
            }

            for ( size_t i { exp_bit_size + 1uz }; i < m_size; i++ )
            {
                if ( i == exp_bit_size + 1uz && other_bits[i] == 1 )
                {
                    other_Mantissa += static_cast<__FLOAT_TYPE>(1);
                } else if ( other_bits[i] == 1 )
                {
                    other_Mantissa += static_cast<__FLOAT_TYPE>(1) / static_cast<__FLOAT_TYPE>(std::pow(2, i-(exp_bit_size+1uz)));
                }
            }

            if ( this_Exponent >= 1023.0 )
            {
                this_Exponent -= 1023.0;
            } else if ( other_Exponent >= 1023.0 )
            {
                other_Exponent -= 1023.0;
            }
            std::cout << "THIS: " << this_Exponent << " " << this_Mantissa << std::endl;
            std::cout << "OTHER: " << other_Exponent << " " << other_Mantissa << std::endl;

            std::cout << std::ldexp(this_Mantissa, std::pow(2, (this_Exponent - 1023.0))) << std::endl;
            std::cout << std::ldexp(other_Mantissa, std::pow(2, (other_Exponent - 1023.0))) << std::endl;

            if ( other_Mantissa == static_cast<__FLOAT_TYPE>(1) && this_Mantissa == static_cast<__FLOAT_TYPE>(1) )
            {
                __FLOAT_TYPE this_Value = (this_bits[0] == 1) ?
                -(this_Mantissa * std::pow(2, this_Exponent) - static_cast<__FLOAT_TYPE>(1)) :
                this_Mantissa * std::pow(2, this_Exponent) - static_cast<__FLOAT_TYPE>(1);

                __FLOAT_TYPE other_Value = (other_bits[0] == 1) ?
                -(other_Mantissa * std::pow(2, other_Exponent) - static_cast<__FLOAT_TYPE>(1)) :
                other_Mantissa * std::pow(2, other_Exponent) - static_cast<__FLOAT_TYPE>(1);

                return this_Value + other_Value;
            } else if ( other_Mantissa == static_cast<__FLOAT_TYPE>(1) )
            {
                __FLOAT_TYPE this_Value = (this_bits[0] == 1) ?
                -(((this_Mantissa == static_cast<__FLOAT_TYPE>(0)) ?
                static_cast<__FLOAT_TYPE>(0.5) :
                this_Mantissa) * std::pow(2, this_Exponent)) :
                ((this_Mantissa == static_cast<__FLOAT_TYPE>(0)) ?
                static_cast<__FLOAT_TYPE>(0.5) :
                this_Mantissa) * std::pow(2, this_Exponent);

                __FLOAT_TYPE other_Value = (other_bits[0] == 1) ?
                -(other_Mantissa * std::pow(2, other_Exponent) - static_cast<__FLOAT_TYPE>(1)) :
                other_Mantissa * std::pow(2, other_Exponent) - static_cast<__FLOAT_TYPE>(1);

                return this_Value + other_Value;
            } else if ( this_Mantissa == static_cast<__FLOAT_TYPE>(1) )
            {
                __FLOAT_TYPE this_Value = (this_bits[0] == 1) ?
                -(this_Mantissa * std::pow(2, this_Exponent) - static_cast<__FLOAT_TYPE>(1)) :
                this_Mantissa * std::pow(2, this_Exponent) - static_cast<__FLOAT_TYPE>(1);

                __FLOAT_TYPE other_Value = (other_bits[0] == 1) ?
                -(((other_Mantissa == static_cast<__FLOAT_TYPE>(0)) ?
                static_cast<__FLOAT_TYPE>(0.5) :
                other_Mantissa) * std::pow(2, other_Exponent)) :
                ((other_Mantissa == static_cast<__FLOAT_TYPE>(0)) ?
                static_cast<__FLOAT_TYPE>(0.5) :
                other_Mantissa) * std::pow(2, other_Exponent);

                return this_Value + other_Value;
            } else
            {
                __FLOAT_TYPE this_Value = (this_bits[0] == 1) ?
                -(((this_Mantissa == static_cast<__FLOAT_TYPE>(0)) ?
                static_cast<__FLOAT_TYPE>(0.5) :
                this_Mantissa) * std::pow(2, this_Exponent)) :
                ((this_Mantissa == static_cast<__FLOAT_TYPE>(0)) ?
                static_cast<__FLOAT_TYPE>(0.5) :
                this_Mantissa) * std::pow(2, this_Exponent);

                __FLOAT_TYPE other_Value = (other_bits[0] == 1) ?
                -(((other_Mantissa == static_cast<__FLOAT_TYPE>(0)) ?
                static_cast<__FLOAT_TYPE>(0.5) :
                other_Mantissa) * std::pow(2, other_Exponent)) :
                ((other_Mantissa == static_cast<__FLOAT_TYPE>(0)) ?
                static_cast<__FLOAT_TYPE>(0.5) :
                other_Mantissa) * std::pow(2, other_Exponent);

                return this_Value + other_Value;
            }
        } else
        {
            return 1.0;
        }
    }
};
