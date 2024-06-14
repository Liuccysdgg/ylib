// MIT License﻿
// Copyright(c) 2024 FastWeb - fwlua.com - nianhua﻿
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :﻿
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.﻿
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.﻿
// ## Additional Terms for Commercial Use﻿
// This software is licensed for personal, educational, and non - commercial use.
// For commercial use or use within a company, organization, or institution, a
// separate commercial license is required.To obtain a commercial license,
// please contact
// EMail：1585346868@qq.com

// QQ：1585346868

#include "util/codec.h"
#include "base/define.h"
#include <locale>
#include <codecvt>
#include <sstream>

#include "zlib/zlib.h"
#include "HPSocket/HPSocket.h"
#include <openssl/evp.h>
#ifdef _WIN32
#include <windows.h>
#endif
#include <iomanip>
#include <cstring>


std::string ylib::codec::md5(const ylib::buffer &input)
{
    EVP_MD_CTX* mdctx;
    const EVP_MD* md;
    unsigned char md_value[EVP_MAX_MD_SIZE];
    unsigned int md_len;

    mdctx = EVP_MD_CTX_new();
    md = EVP_md5();
    EVP_DigestInit_ex(mdctx, md, NULL);
    EVP_DigestUpdate(mdctx, input.data(), input.size());
    EVP_DigestFinal_ex(mdctx, md_value, &md_len);
    EVP_MD_CTX_free(mdctx);
    std::ostringstream hexStream;
    hexStream << std::hex << std::setfill('0');
    for (unsigned int i = 0; i < md_len; i++) {
        hexStream << std::setw(2) << (int)md_value[i];
    }
    return hexStream.str();
}

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

std::string codec::url::en(const buffer& data)
{
    DWORD resultLen = SYS_GuessUrlEncodeBound((const BYTE*)data.data(),(DWORD)data.length());
    if (resultLen <= 0)
        return ylib::buffer();
    if (resultLen > 8589934592)
        return ylib::buffer();
    ylib::buffer result;
    result.resize(resultLen);
    auto rcode = SYS_UrlEncode((BYTE*)data.data(), (DWORD)data.length(), (BYTE*)result.data(), resultLen);
    return result.left(resultLen);
}

ylib::buffer codec::url::de(const std::string& data)
{
    DWORD resultLen = SYS_GuessUrlDecodeBound((const BYTE*)data.c_str(), (DWORD)data.length());
    if (resultLen <= 0)
        return ylib::buffer();
    if (resultLen > 8589934592)
        return ylib::buffer();
    ylib::buffer result;
    result.resize(resultLen);
    auto rcode = SYS_UrlDecode((BYTE*)data.c_str(), (DWORD)data.length(), (BYTE*)result.data(), resultLen);
    return result.left(resultLen);
}





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