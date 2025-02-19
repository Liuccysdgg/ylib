/*Software License

Copyright(C) 2024[liuyingjie]
License Terms
Usage Rights

Any individual or entity is free to use, copy, and distribute the binary form of this software without modification to the source code, without the need to disclose the source code.
If the source code is modified, the modifications must be open - sourced under the same license.This means that the modifications must be disclosed and accompanied by a copy of this license.
Future Versions Updates
From this version onwards, all future releases will be governed by the terms of the latest version of the license.This license will automatically be nullified and replaced by the new version.
Users must comply with the terms of the new license issued in future releases.
Liability and Disclaimer
This software is provided “as is”, without any express or implied warranties, including but not limited to the warranties of merchantability, fitness for a particular purpose, and non - infringement.In no event shall the author or copyright holder be liable for any claims, damages, or other liabilities, whether in an action of contract, tort, or otherwise, arising from, out of, or in connection with the software or the use or other dealings in the software.
Contact Information
If you have any questions, please contact us: 1585346868@qq.com Or visit our website fwlua.com.
*/

#include "util/codec.h"
#include "base/define.h"
#include <locale>
#include <codecvt>
#include <sstream>

#include "zlib/zlib.h"
#include "HPSocket/HPSocket.h"
#if USE_OPENSSL== 1
#include <openssl/evp.h>
#include <openssl/sha.h>
#endif
#ifdef _WIN32
#include <windows.h>
#endif
#include <iomanip>
#include <cstring>
#if USE_OPENSSL== 1
ylib::buffer ylib::codec::sha1(const ylib::buffer& data)
{
    ylib::buffer result;
    result.resize(20);
    SHA_CTX sha1;
    SHA1_Init(&sha1);
    SHA1_Update(&sha1, data.data(), data.size());
    SHA1_Final((unsigned char*)result.data(), &sha1);

    return result;
    //std::stringstream ss;
    //for (int i = 0; i < SHA_DIGEST_LENGTH; i++) {
    //    ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    //}
    //return ss.str();
}
#endif
std::string ylib::codec::to_utf8(const std::string &gbk)
{
#ifdef _WIN32
    //gbk转unicode
    int len = MultiByteToWideChar(CP_ACP, 0, gbk.c_str(), -1, NULL, 0);
    wchar_t* strUnicode = new wchar_t[len];
    wmemset(strUnicode, 0, len);
    MultiByteToWideChar(CP_ACP, 0, gbk.c_str(), -1, strUnicode, len);

    //unicode转UTF-8
    len = WideCharToMultiByte(CP_UTF8, 0, strUnicode, -1, NULL, 0, NULL, NULL);
    char* strUtf8 = new char[len+1];
    strUtf8[len] = 0;
    WideCharToMultiByte(CP_UTF8, 0, strUnicode, -1, strUtf8, len, NULL, NULL);

    std::string strTemp(strUtf8);//此时的strTemp是UTF-8编码
    delete[]strUnicode;
    delete[]strUtf8;
    strUnicode = NULL;
    strUtf8 = NULL;
    return strTemp;

#else
    char *dst = new char[gbk.length()*4];
    int dst_length = gbk.length()*4;
    if(SYS_GbkToUtf8(gbk.c_str(),dst,dst_length) == false)
        return "";
    return std::string(dst,dst_length);
#endif
}

std::string ylib::codec::to_gbk(const std::string &utf8)
{
#ifdef _WIN32
    //UTF-8转unicode
    int len = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, NULL, 0);
    wchar_t* strUnicode = new wchar_t[len];//len = 2
    wmemset(strUnicode, 0, len);
    MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, strUnicode, len);

    //unicode转gbk
    len = WideCharToMultiByte(CP_ACP, 0, strUnicode, -1, NULL, 0, NULL, NULL);
    char* strGbk = new char[len+1];//len=3 本来为2，但是char*后面自动加上了\0
    memset(strGbk, 0, len);
    WideCharToMultiByte(CP_ACP, 0, strUnicode, -1, strGbk, len, NULL, NULL);
    strGbk[len] = 0;
    std::string strTemp(strGbk);//此时的strTemp是GBK编码
    delete[]strUnicode;
    delete[]strGbk;
    strUnicode = NULL;
    strGbk = NULL;
    return strTemp;
#else
    char *dst = new char[utf8.length()*4];
    int dst_length = utf8.length()*4;
    if(SYS_Utf8ToGbk(utf8.c_str(),dst,dst_length) == false)
        return "";
    return std::string(dst,dst_length);
#endif
}

ylib::buffer ylib::codec::gzip::en(const buffer &data)
{
    ylib::buffer result;
    result.resize(data.length()+1024);
    DWORD resultLen = (DWORD)result.length();
    if (SYS_GZipCompress((const BYTE*)data.data(), (DWORD)data.length(), (BYTE*)result.data(), resultLen) == 0)
        return result.left(resultLen);
    return result;
}
ylib::buffer ylib::codec::gzip::de(const buffer &data)
{
    uint32 size = SYS_GZipGuessUncompressBound((const BYTE*)data.data(),(DWORD)data.length());
    if (size <= 0)
        return ylib::buffer();
    if (size > 8589934592)
        return ylib::buffer();
    ylib::buffer result;
    result.resize(size*2);
    DWORD resultLen = size*2;
    SYS_GZipUncompress((const BYTE*)data.data(), (DWORD)data.length(), (BYTE*)result.data(), resultLen);
    return result.left(resultLen);
}

std::string codec::base64::en(const buffer &data)
{
    DWORD resultLen = SYS_GuessBase64EncodeBound((DWORD)data.length());
    if (resultLen <= 0)
        return ylib::buffer();
    if (resultLen > 8589934592)
        return ylib::buffer();
    ylib::buffer result;
    result.resize(resultLen);
    auto rcode = SYS_Base64Encode((const BYTE*)data.data(), (DWORD)data.length(), (BYTE*)result.data(), resultLen);
    return result.left(resultLen);
}

ylib::buffer codec::base64::de(const std::string &data)
{
    DWORD resultLen = SYS_GuessBase64DecodeBound((const BYTE*)data.c_str(), (DWORD)data.length());
    if (resultLen <= 0)
        return ylib::buffer();
    if (resultLen > 8589934592)
        return ylib::buffer();
    ylib::buffer result;
    result.resize(resultLen);
    auto rcode = SYS_Base64Decode((const BYTE*)data.c_str(), (DWORD)data.length(), (BYTE*)result.data(), resultLen);
    return result.left(resultLen);
}

std::string codec::url::en(const std::string& data)
{
    std::ostringstream encoded;
    for (char c : data) {
        // 保留字母、数字和特定符号
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            encoded << c;
        }
        else {
            // 对其他字符进行编码为 %xx 格式
            encoded << '%' << std::uppercase << std::hex << static_cast<int>(static_cast<unsigned char>(c));
        }
    }
    return encoded.str();
}

ylib::buffer codec::url::de(const std::string& str)
{
    std::ostringstream decoded;
    for (size_t i = 0; i < str.length(); ++i) {
        char c = str[i];
        if (c == '%') {
            // 确保后面至少有两个字符供解析
            if (i + 2 >= str.length()) {
                throw std::invalid_argument("Invalid URL encoding: incomplete percent-encoded sequence.");
            }
            // 获取两个十六进制字符
            char hex[3] = { str[i + 1], str[i + 2], '\0' };
            if (!isxdigit(hex[0]) || !isxdigit(hex[1])) {
                throw std::invalid_argument("Invalid URL encoding: invalid hex digits.");
            }
            // 转换为实际字符
            decoded << static_cast<char>(std::strtol(hex, nullptr, 16));
            i += 2; // 跳过已处理的两个字符
        }
        else if (c == '+') {
            // '+' 在 URL 编码中表示空格
            decoded << ' ';
        }
        else {
            // 保留普通字符
            decoded << c;
        }
    }
    return decoded.str();
}




#if USE_OPENSSL== 1
ylib::buffer codec::aes::en(const ylib::buffer& data, const std::string& key, ylib::codec::aes::variant var, ylib::codec::aes::mode mod)
{
    try
    {
        const EVP_CIPHER* cipher;
        unsigned char keyFixed[EVP_MAX_KEY_LENGTH] = { 0 }; // 最大密钥长度
        std::memcpy(keyFixed, key.data(), ylib_min(key.size(), sizeof(keyFixed)));

        switch (var) {
        case variant::AES128:
            cipher = (mod == mode::CBC) ? EVP_aes_128_cbc() : EVP_aes_128_ecb();
            break;
        case variant::AES192:
            cipher = (mod == mode::CBC) ? EVP_aes_192_cbc() : EVP_aes_192_ecb();
            break;
        case variant::AES256:
            cipher = (mod == mode::CBC) ? EVP_aes_256_cbc() : EVP_aes_256_ecb();
            break;
        default:
            throw std::runtime_error("Unsupported AES variant");
        }

        EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
        if (!ctx) {
            std::cerr << "Failed to create EVP_CIPHER_CTX" << std::endl;
            throw std::runtime_error("Failed to create EVP_CIPHER_CTX");
        }

        if (1 != EVP_EncryptInit_ex(ctx, cipher, NULL, keyFixed, NULL)) {
            std::cerr << "Failed to initialize AES encryption" << std::endl;
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("Failed to initialize AES encryption");
        }

        std::vector<unsigned char> out_buf(data.size() + EVP_MAX_BLOCK_LENGTH);
        int out_len1, out_len2;

        if (1 != EVP_EncryptUpdate(ctx, out_buf.data(), &out_len1, reinterpret_cast<const unsigned char*>(data.data()), (int)data.length())) {
            std::cerr << "Failed to encrypt data" << std::endl;
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("Failed to encrypt data");
        }

        if (1 != EVP_EncryptFinal_ex(ctx, out_buf.data() + out_len1, &out_len2)) {
            std::cerr << "Failed to finalize AES encryption" << std::endl;
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("Failed to finalize AES encryption");
        }

        EVP_CIPHER_CTX_free(ctx);

        out_buf.resize(out_len1 + out_len2);
        return ylib::buffer(reinterpret_cast<const char*>(out_buf.data()), out_buf.size());
    }
    catch (const std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }
    return ylib::buffer();
    
}
ylib::buffer codec::aes::de(const ylib::buffer& ciphertext, const std::string& key, variant var, mode mod)
{
    try
    {
        const EVP_CIPHER* cipher;
        unsigned char keyFixed[EVP_MAX_KEY_LENGTH] = { 0 }; // 最大密钥长度
        std::memcpy(keyFixed, key.data(), ylib_min(key.size(), sizeof(keyFixed)));

        switch (var) {
        case variant::AES128:
            cipher = (mod == mode::CBC) ? EVP_aes_128_cbc() : EVP_aes_128_ecb();
            break;
        case variant::AES192:
            cipher = (mod == mode::CBC) ? EVP_aes_192_cbc() : EVP_aes_192_ecb();
            break;
        case variant::AES256:
            cipher = (mod == mode::CBC) ? EVP_aes_256_cbc() : EVP_aes_256_ecb();
            break;
        default:
            throw std::runtime_error("Unsupported AES variant");
        }

        EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
        if (!ctx) {
            std::cerr << "Failed to create EVP_CIPHER_CTX" << std::endl;
            throw std::runtime_error("Failed to create EVP_CIPHER_CTX");
        }

        if (1 != EVP_DecryptInit_ex(ctx, cipher, NULL, keyFixed, NULL)) {
            std::cerr << "Failed to initialize AES decryption" << std::endl;
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("Failed to initialize AES decryption");
        }

        std::vector<unsigned char> out_buf(ciphertext.length() + EVP_MAX_BLOCK_LENGTH);
        int out_len1, out_len2;

        if (1 != EVP_DecryptUpdate(ctx, out_buf.data(), &out_len1, reinterpret_cast<const unsigned char*>(ciphertext.data()), (int)ciphertext.length())) {
            std::cerr << "Failed to decrypt data" << std::endl;
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("Failed to decrypt data");
        }

        if (1 != EVP_DecryptFinal_ex(ctx, out_buf.data() + out_len1, &out_len2)) {
            std::cerr << "Failed to finalize AES decryption" << std::endl;
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("Failed to finalize AES decryption");
        }

        EVP_CIPHER_CTX_free(ctx);

        out_buf.resize(out_len1 + out_len2);
        return ylib::buffer(reinterpret_cast<const char*>(out_buf.data()), out_buf.size());
    }
    catch (const std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }
    return ylib::buffer();
}
#endif