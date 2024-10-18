#pragma once

#include <iostream>
#include <algorithm>
#include <vector>
#include <list>
#include <string>
#include <map>
#include <set>

namespace Vanish
{
    namespace Serialize
    {
        class ISerializable;
        enum DataType
        {
            BOOL = 0,
            CHAR,
            INT32,
            INT64,
            FLOAT,
            DOUBLE,
            STRING,
            VECTOR,
            LIST,
            MAP,
            SET,
            CUSTOM
        };
        enum ByteOrder
        {
            LITTLE_ENDIAN = 0,
            BIG_ENDIAN
        };

        class DataStream
        {
        private:
            std::vector<char> m_buffer;
            int m_position = 0;
            ByteOrder m_byteOrder;

        public:
            DataStream() {m_byteOrder = GetSystemByteOrder();}
            ~DataStream() {}

        public:
            void Show() const;

        public:
            void Write(bool data);
            void Write(char data);
            void Write(int32_t data);
            void Write(int64_t data);
            void Write(float data);
            void Write(double data);
            void Write(const std::string &data);

            bool Read(bool &data);
            bool Read(char &data);
            bool Read(int32_t &data);
            bool Read(int64_t &data);
            bool Read(float &data);
            bool Read(double &data);
            bool Read(std::string &data);

            DataStream &operator<<(bool data);
            DataStream &operator<<(char data);
            DataStream &operator<<(int32_t data);
            DataStream &operator<<(int64_t data);
            DataStream &operator<<(float data);
            DataStream &operator<<(double data);
            DataStream &operator<<(const std::string &data);

            DataStream &operator>>(bool &data);
            DataStream &operator>>(char &data);
            DataStream &operator>>(int32_t &data);
            DataStream &operator>>(int64_t &data);
            DataStream &operator>>(float &data);
            DataStream &operator>>(double &data);
            DataStream &operator>>(std::string &data);

        public:
            template <typename T>
            void Write(const std::vector<T> &data);
            template <typename T>
            void Write(const std::list<T> &data);
            template <typename K, typename V>
            void Write(const std::map<K, V> &data);
            template <typename T>
            void Write(const std::set<T> &data);

            template <typename T>
            bool Read(std::vector<T> &data);
            template <typename T>
            bool Read(std::list<T> &data);
            template <typename K, typename V>
            bool Read(std::map<K, V> &data);
            template <typename T>
            bool Read(std::set<T> &data);

            template <typename T>
            DataStream &operator<<(const std::vector<T> &data);
            template <typename T>
            DataStream &operator<<(const std::list<T> &data);
            template <typename K, typename V>
            DataStream &operator<<(const std::map<K, V> &data);
            template <typename T>
            DataStream &operator<<(const std::set<T> &data);

            template <typename T>
            DataStream &operator>>(std::vector<T> &data);
            template <typename T>
            DataStream &operator>>(std::list<T> &data);
            template <typename K, typename V>
            DataStream &operator>>(std::map<K, V> &data);
            template <typename T>
            DataStream &operator>>(std::set<T> &data);

        public:
            void Write(ISerializable &data);
            bool Read(ISerializable &data);

            DataStream &operator<<(ISerializable &data);
            DataStream &operator>>(ISerializable &data);

        public:
            template <typename T, typename... Args>
            void Write_args(const T &data, const Args &...args);
            template <typename T, typename... Args>
            bool Read_args(T &data, Args &...args);

            void Write_args() {}
            bool Read_args() { return true; }

        private:
            void Write(const char *data, int length);
            void Reserve(int length); // 自己控制扩容以减少频繁写入导致频繁扩容
            ByteOrder GetSystemByteOrder();
        };

        void DataStream::Show() const
        {
            int size = m_buffer.size();
            std::cout << "DataStream size: " << size << std::endl;
            for (int i = 0; i < size; i++)
            {
                std::cout << m_buffer[i];
            }
            std::cout << std::endl;
        }

        void DataStream::Reserve(int length)
        {
            int size = m_buffer.size();
            int capacity = m_buffer.capacity();
            if (size + length > capacity)
            {
                while (size + length > capacity)
                {
                    if (capacity == 0)
                    {
                        capacity = 1;
                    }
                    else
                    {
                        capacity *= 2;
                    }
                }
                m_buffer.reserve(capacity); // 确保容量足够 reserve函数: 调整底层数组的大小，使其至少可以容纳capacity个元素
            }
        }

        ByteOrder DataStream::GetSystemByteOrder()
        {
            int n = 0x12345678;
            char *p = (char *)&n;
            if (p[0] == 0x12)
            {
                return LITTLE_ENDIAN;
            }
            else
            {
                return BIG_ENDIAN;
            }
        }

        void DataStream::Write(const char *data, int length)
        {
            Reserve(length); // 先保证容量足够
            int size = m_buffer.size();
            m_buffer.resize(size + length);             // resize函数: 调整底层数组的大小，使其至少可以容纳length个元素
            std::memcpy(&m_buffer[size], data, length); // memcpy函数: 将data的前length个字节拷贝到m_buffer的后size个字节处
        }

        void DataStream::Write(bool data)
        {
            char type = DataType::BOOL;
            Write((char *)&type, sizeof(char)); // 写入数据类型
            Write((char *)&data, sizeof(bool)); // 写入数据
        }
        void DataStream::Write(char data)
        {
            char type = DataType::CHAR;
            Write((char *)&type, sizeof(char)); // 写入数据类型
            Write((char *)&data, sizeof(char)); // 写入数据
        }
        void DataStream::Write(int32_t data)
        {
            char type = DataType::INT32;
            Write((char *)&type, sizeof(char));    // 写入数据类型
            if(m_byteOrder == ByteOrder::BIG_ENDIAN)
            {
                char *first = (char *)&data;
                char *last = first + sizeof(int32_t);
                std::reverse(first, last); // 字节序转换
            }
            Write((char *)&data, sizeof(int32_t)); // 写入数据
        }
        void DataStream::Write(int64_t data)
        {
            char type = DataType::INT64;
            Write((char *)&type, sizeof(char));    // 写入数据类型
            if(m_byteOrder == ByteOrder::BIG_ENDIAN)
            {
                char *first = (char *)&data;
                char *last = first + sizeof(int64_t);
                std::reverse(first, last); // 字节序转换
            }
            Write((char *)&data, sizeof(int64_t)); // 写入数据
        }
        void DataStream::Write(float data)
        {
            char type = DataType::FLOAT;
            Write((char *)&type, sizeof(char));  // 写入数据类型
            if(m_byteOrder == ByteOrder::BIG_ENDIAN)
            {
                char *first = (char *)&data;
                char *last = first + sizeof(float);
                std::reverse(first, last); // 字节序转换
            }
            Write((char *)&data, sizeof(float)); // 写入数据
        }
        void DataStream::Write(double data)
        {
            char type = DataType::DOUBLE;
            Write((char *)&type, sizeof(char));   // 写入数据类型
            if(m_byteOrder == ByteOrder::BIG_ENDIAN)
            {
                char *first = (char *)&data;
                char *last = first + sizeof(double);
                std::reverse(first, last); // 字节序转换
            }
            Write((char *)&data, sizeof(double)); // 写入数据
        }
        void DataStream::Write(const std::string &data)
        {
            char type = DataType::STRING;
            Write((char *)&type, sizeof(char)); // 写入数据类型
            int32_t length = data.length();
            Write(length); // 写入字符串长度
            Write(data.c_str(), length);              // 写入字符串内容
        }
        bool DataStream::Read(bool &data)
        {
            if (m_buffer[m_position] != DataType::BOOL)
            {
                return false;
            }
            ++m_position;
            data = *((bool *)&m_buffer[m_position++]);
            return true;
        }
        bool DataStream::Read(char &data)
        {
            if (m_buffer[m_position] != DataType::CHAR)
            {
                return false;
            }
            ++m_position;
            data = *((char *)&m_buffer[m_position++]);
            return true;
        }
        bool DataStream::Read(int32_t &data)
        {
            if (m_buffer[m_position] != DataType::INT32)
            {
                return false;
            }
            ++m_position;
            data = *((int32_t *)&m_buffer[m_position]);
            if(m_byteOrder == ByteOrder::BIG_ENDIAN)
            {
                char *first = (char *)&data;
                char *last = first + sizeof(int32_t);
                std::reverse(first, last); // 字节序转换
            }
            m_position += sizeof(int32_t);
            return true;
        }
        bool DataStream::Read(int64_t &data)
        {
            if (m_buffer[m_position] != DataType::INT64)
            {
                return false;
            }
            ++m_position;
            data = *((int64_t *)&m_buffer[m_position]);
            if(m_byteOrder == ByteOrder::BIG_ENDIAN)
            {
                char *first = (char *)&data;
                char *last = first + sizeof(int32_t);
                std::reverse(first, last); // 字节序转换
            }
            m_position += sizeof(int64_t);
            return true;
        }
        bool DataStream::Read(float &data)
        {
            if (m_buffer[m_position] != DataType::FLOAT)
            {
                return false;
            }
            ++m_position;
            data = *((float *)&m_buffer[m_position]);
            if(m_byteOrder == ByteOrder::BIG_ENDIAN)
            {
                char *first = (char *)&data;
                char *last = first + sizeof(int32_t);
                std::reverse(first, last); // 字节序转换
            }
            m_position += sizeof(float);
            return true;
        }
        bool DataStream::Read(double &data)
        {
            if (m_buffer[m_position] != DataType::DOUBLE)
            {
                return false;
            }
            ++m_position;
            data = *((double *)&m_buffer[m_position]);
            if(m_byteOrder == ByteOrder::BIG_ENDIAN)
            {
                char *first = (char *)&data;
                char *last = first + sizeof(int32_t);
                std::reverse(first, last); // 字节序转换
            }
            m_position += sizeof(double);
            return true;
        }
        bool DataStream::Read(std::string &data)
        {
            if (m_buffer[m_position] != DataType::STRING)
            {
                return false;
            }

            ++m_position;
            int32_t length = 0;
            Read(length);
            if (length <= 0)
            {
                return false;
            }
            data.assign(&m_buffer[m_position], length); // 将m_buffer的m_position开始的length个字节赋值给data
            m_position += length;
            return true;
        }

        DataStream &DataStream::operator<<(bool data)
        {
            Write(data);
            return *this;
        }
        DataStream &DataStream::operator<<(char data)
        {
            Write(data);
            return *this;
        }
        DataStream &DataStream::operator<<(int32_t data)
        {
            Write(data);
            return *this;
        }
        DataStream &DataStream::operator<<(int64_t data)
        {
            Write(data);
            return *this;
        }
        DataStream &DataStream::operator<<(float data)
        {
            Write(data);
            return *this;
        }
        DataStream &DataStream::operator<<(double data)
        {
            Write(data);
            return *this;
        }
        DataStream &DataStream::operator<<(const std::string &data)
        {
            Write(data);
            return *this;
        }
        DataStream &DataStream::operator>>(bool &data)
        {
            Read(data);
            return *this;
        }
        DataStream &DataStream::operator>>(char &data)
        {
            Read(data);
            return *this;
        }
        DataStream &DataStream::operator>>(int32_t &data)
        {
            Read(data);
            return *this;
        }
        DataStream &DataStream::operator>>(int64_t &data)
        {
            Read(data);
            return *this;
        }
        DataStream &DataStream::operator>>(float &data)
        {
            Read(data);
            return *this;
        }
        DataStream &DataStream::operator>>(double &data)
        {
            Read(data);
            return *this;
        }
        DataStream &DataStream::operator>>(std::string &data)
        {
            Read(data);
            return *this;
        }

        template <typename T>
        void DataStream::Write(const std::vector<T> &data)
        {
            char type = DataType::VECTOR;
            Write((char *)&type, sizeof(char)); // 写入数据类型
            int64_t length = data.size();
            Write(length); // 写入数据长度
            for (int i = 0; i < length; i++)
            {
                Write(data[i]); // 写入数据内容
            }
        }
        template <typename T>
        void DataStream::Write(const std::list<T> &data)
        {
            char type = DataType::LIST;
            Write((char *)&type, sizeof(char)); // 写入数据类型
            int64_t length = data.size();
            Write(length); // 写入数据长度
            for (auto it = data.begin(); it != data.end(); it++)
            {
                Write(*it); // 写入数据内容
            }
        }
        template <typename K, typename V>
        void DataStream::Write(const std::map<K, V> &data)
        {
            char type = DataType::MAP;
            Write((char *)&type, sizeof(char)); // 写入数据类型
            int64_t length = data.size();
            Write(length); // 写入数据长度
            for (auto it = data.begin(); it != data.end(); it++)
            {
                Write(it->first);  // 写入键
                Write(it->second); // 写入值
            }
        }
        template <typename T>
        void DataStream::Write(const std::set<T> &data)
        {
            char type = DataType::SET;
            Write((char *)&type, sizeof(char)); // 写入数据类型
            int64_t length = data.size();
            Write(length); // 写入数据长度
            for (auto it = data.begin(); it != data.end(); it++)
            {
                Write(*it); // 写入数据内容
            }
        }

        template <typename T>
        bool DataStream::Read(std::vector<T> &data)
        {
            if (m_buffer[m_position] != DataType::VECTOR)
            {
                return false;
            }
            ++m_position;
            int64_t length = 0;
            Read(length);
            data.resize(length);
            for (int i = 0; i < length; i++)
            {
                T t;
                Read(t);
                data.emplace_back(t);
            }
            return true;
        }
        template <typename T>
        bool DataStream::Read(std::list<T> &data)
        {
            if (m_buffer[m_position] != DataType::LIST)
            {
                return false;
            }
            ++m_position;
            int64_t length = 0;
            Read(length);
            for (int i = 0; i < length; i++)
            {
                T t;
                Read(t);
                data.emplace_back(t);
            }
            return true;
        }
        template <typename K, typename V>
        bool DataStream::Read(std::map<K, V> &data)
        {
            if (m_buffer[m_position] != DataType::MAP)
            {
                return false;
            }
            ++m_position;
            int64_t length = 0;
            Read(length);
            for (int i = 0; i < length; i++)
            {
                K k;
                V v;
                Read(k);
                Read(v);
                data.emplace(k, v);
            }
            return true;
        }
        template <typename T>
        bool DataStream::Read(std::set<T> &data)
        {
            if (m_buffer[m_position] != DataType::SET)
            {
                return false;
            }
            ++m_position;
            int64_t length = 0;
            Read(length);
            for (int i = 0; i < length; i++)
            {
                T t;
                Read(t);
                data.emplace(t);
            }
            return true;
        }

        template <typename T>
        DataStream &DataStream::operator<<(const std::vector<T> &data)
        {
            Write(data);
            return *this;
        }
        template <typename T>
        DataStream &DataStream::operator<<(const std::list<T> &data)
        {
            Write(data);
            return *this;
        }
        template <typename K, typename V>
        DataStream &DataStream::operator<<(const std::map<K, V> &data)
        {
            Write(data);
            return *this;
        }
        template <typename T>
        DataStream &DataStream::operator<<(const std::set<T> &data)
        {
            Write(data);
            return *this;
        }

        template <typename T>
        DataStream &DataStream::operator>>(std::vector<T> &data)
        {
            Read(data);
            return *this;
        }
        template <typename T>
        DataStream &DataStream::operator>>(std::list<T> &data)
        {
            Read(data);
            return *this;
        }
        template <typename K, typename V>
        DataStream &DataStream::operator>>(std::map<K, V> &data)
        {
            Read(data);
            return *this;
        }
        template <typename T>
        DataStream &DataStream::operator>>(std::set<T> &data)
        {
            Read(data);
            return *this;
        }

        void DataStream::Write(ISerializable &data)
        {
            data.Serialize(*this);
        }
        bool DataStream::Read(ISerializable &data)
        {
            data.Deserialize(*this);
            return true;
        }

        DataStream &DataStream::operator<<(ISerializable &data)
        {
            Write(data);
            return *this;
        }
        DataStream &DataStream::operator>>(ISerializable &data)
        {
            Read(data);
            return *this;
        }

        template <typename T, typename... Args>
        void DataStream::Write_args(const T &data, const Args &...args)
        {
            Write(data);
            Write_args(args...);
        }
        template <typename T, typename... Args>
        bool DataStream::Read_args(T &data, Args &...args)
        {
            if (!Read(data))
            {
                return false;
            }
            return Read_args(args...);
        }

        class ISerializable
        {
        public:
            virtual void Serialize(DataStream &stream) = 0;
            virtual void Deserialize(DataStream &stream) = 0;

        #define SERIALIZE_FUNC(...)                        \
            void serialize(DataStream &stream)             \
            {                                              \
                char type = DataType::CUSTOM;              \
                stream.Write((char *)&type, sizeof(char)); \
                stream.Write_args(__VA_ARGS__);            \
            }                                              \
            bool deserialize(DataStream &stream)           \
            {                                              \
                char type;                                 \
                stream.Read((char *)&type, sizeof(char));  \
                if (type != DataType::CUSTOM)              \
                {                                          \
                    return false;                          \
                }                                          \
                stream.Read_args(__VA_ARGS__);             \
                return true;                               \
            }
        };
    }
}