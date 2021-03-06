// Copyright 2016 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/*!
 * @file MeasurementPacket.cpp
 * This source file contains the definition of the described types in the IDL file.
 *
 * This file was generated by the tool gen.
 */

#ifdef _WIN32
// Remove linker warning LNK4221 on Visual Studio
namespace {
char dummy;
}  // namespace
#endif  // _WIN32

#include "MeasurementPacket.h"
#include <fastcdr/Cdr.h>

#include <fastcdr/exceptions/BadParamException.h>
using namespace eprosima::fastcdr::exception;

#include <utility>

MeasurementPacket::MeasurementPacket()
{
    // m_p_type com.eprosima.idl.parser.typecode.PrimitiveTypeCode@61f8bee4
    m_p_type = 0;
    // m_p_textmessage com.eprosima.idl.parser.typecode.StringTypeCode@7b49cea0
    m_p_textmessage ="";
    // m_id com.eprosima.idl.parser.typecode.PrimitiveTypeCode@887af79
    m_id = 0;
    // m_timestamp com.eprosima.idl.parser.typecode.PrimitiveTypeCode@7fac631b
    m_timestamp = 0;

}

MeasurementPacket::~MeasurementPacket()
{




}

MeasurementPacket::MeasurementPacket(
        const MeasurementPacket& x)
{
    m_p_type = x.m_p_type;
    m_p_textmessage = x.m_p_textmessage;
    m_id = x.m_id;
    m_timestamp = x.m_timestamp;
}

MeasurementPacket::MeasurementPacket(
        MeasurementPacket&& x)
{
    m_p_type = x.m_p_type;
    m_p_textmessage = std::move(x.m_p_textmessage);
    m_id = x.m_id;
    m_timestamp = x.m_timestamp;
}

MeasurementPacket& MeasurementPacket::operator =(
        const MeasurementPacket& x)
{

    m_p_type = x.m_p_type;
    m_p_textmessage = x.m_p_textmessage;
    m_id = x.m_id;
    m_timestamp = x.m_timestamp;

    return *this;
}

MeasurementPacket& MeasurementPacket::operator =(
        MeasurementPacket&& x)
{

    m_p_type = x.m_p_type;
    m_p_textmessage = std::move(x.m_p_textmessage);
    m_id = x.m_id;
    m_timestamp = x.m_timestamp;

    return *this;
}

size_t MeasurementPacket::getMaxCdrSerializedSize(
        size_t current_alignment)
{
    size_t initial_alignment = current_alignment;


    current_alignment += 2 + eprosima::fastcdr::Cdr::alignment(current_alignment, 2);


    current_alignment += 4 + eprosima::fastcdr::Cdr::alignment(current_alignment, 4) + 255 + 1;

    current_alignment += 4 + eprosima::fastcdr::Cdr::alignment(current_alignment, 4);


    current_alignment += 4 + eprosima::fastcdr::Cdr::alignment(current_alignment, 4);



    return current_alignment - initial_alignment;
}

size_t MeasurementPacket::getCdrSerializedSize(
        const MeasurementPacket& data,
        size_t current_alignment)
{
    (void)data;
    size_t initial_alignment = current_alignment;


    current_alignment += 2 + eprosima::fastcdr::Cdr::alignment(current_alignment, 2);


    current_alignment += 4 + eprosima::fastcdr::Cdr::alignment(current_alignment, 4) + data.p_textmessage().size() + 1;

    current_alignment += 4 + eprosima::fastcdr::Cdr::alignment(current_alignment, 4);


    current_alignment += 4 + eprosima::fastcdr::Cdr::alignment(current_alignment, 4);



    return current_alignment - initial_alignment;
}

void MeasurementPacket::serialize(
        eprosima::fastcdr::Cdr& scdr) const
{

    scdr << m_p_type;
    scdr << m_p_textmessage;
    scdr << m_id;
    scdr << m_timestamp;

}

void MeasurementPacket::deserialize(
        eprosima::fastcdr::Cdr& dcdr)
{

    dcdr >> m_p_type;
    dcdr >> m_p_textmessage;
    dcdr >> m_id;
    dcdr >> m_timestamp;
}

/*!
 * @brief This function sets a value in member p_type
 * @param _p_type New value for member p_type
 */
void MeasurementPacket::p_type(
        uint16_t _p_type)
{
    m_p_type = _p_type;
}

/*!
 * @brief This function returns the value of member p_type
 * @return Value of member p_type
 */
uint16_t MeasurementPacket::p_type() const
{
    return m_p_type;
}

/*!
 * @brief This function returns a reference to member p_type
 * @return Reference to member p_type
 */
uint16_t& MeasurementPacket::p_type()
{
    return m_p_type;
}

/*!
 * @brief This function copies the value in member p_textmessage
 * @param _p_textmessage New value to be copied in member p_textmessage
 */
void MeasurementPacket::p_textmessage(
        const std::string& _p_textmessage)
{
    m_p_textmessage = _p_textmessage;
}

/*!
 * @brief This function moves the value in member p_textmessage
 * @param _p_textmessage New value to be moved in member p_textmessage
 */
void MeasurementPacket::p_textmessage(
        std::string&& _p_textmessage)
{
    m_p_textmessage = std::move(_p_textmessage);
}

/*!
 * @brief This function returns a constant reference to member p_textmessage
 * @return Constant reference to member p_textmessage
 */
const std::string& MeasurementPacket::p_textmessage() const
{
    return m_p_textmessage;
}

/*!
 * @brief This function returns a reference to member p_textmessage
 * @return Reference to member p_textmessage
 */
std::string& MeasurementPacket::p_textmessage()
{
    return m_p_textmessage;
}
/*!
 * @brief This function sets a value in member id
 * @param _id New value for member id
 */
void MeasurementPacket::id(
        uint32_t _id)
{
    m_id = _id;
}

/*!
 * @brief This function returns the value of member id
 * @return Value of member id
 */
uint32_t MeasurementPacket::id() const
{
    return m_id;
}

/*!
 * @brief This function returns a reference to member id
 * @return Reference to member id
 */
uint32_t& MeasurementPacket::id()
{
    return m_id;
}

/*!
 * @brief This function sets a value in member timestamp
 * @param _timestamp New value for member timestamp
 */
void MeasurementPacket::timestamp(
        int32_t _timestamp)
{
    m_timestamp = _timestamp;
}

/*!
 * @brief This function returns the value of member timestamp
 * @return Value of member timestamp
 */
int32_t MeasurementPacket::timestamp() const
{
    return m_timestamp;
}

/*!
 * @brief This function returns a reference to member timestamp
 * @return Reference to member timestamp
 */
int32_t& MeasurementPacket::timestamp()
{
    return m_timestamp;
}


size_t MeasurementPacket::getKeyMaxCdrSerializedSize(
        size_t current_alignment)
{
    size_t current_align = current_alignment;







    return current_align;
}

bool MeasurementPacket::isKeyDefined()
{
    return false;
}

void MeasurementPacket::serializeKey(
        eprosima::fastcdr::Cdr& scdr) const
{
    (void) scdr;
        
}
