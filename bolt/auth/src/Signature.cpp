#include <signature.hpp>
#include <cryptlite/sha256.h>
#include <cryptlite/sha256.h>
#include <cryptlite/hmac.h>
#include <cryptlite/base64.h>
#include <user.hpp>

using namespace cryptlite;

namespace bolt {
	namespace auth {
		Signature::Signature(utility::string_t  auth_string)
		{
			m_auth_string = auth_string;
			

			utility::string_t::size_type pos = m_auth_string.find(':');
			if (m_auth_string.npos != pos) //if a valid signature
			{
				m_username = m_auth_string.substr(0, pos);
				m_signature = m_auth_string.substr(pos + 1);
			}
		}

		Signature::Signature()
		{
		}

		Signature::~Signature()
		{
		}

		bool Signature::isValied(utility::string_t datetime, utility::string_t  resource, utility::string_t  http_method)
		{
			m_datetime = datetime;
			m_resource = resource;
			m_http_method = http_method;
			//Create signature string using headers //base64(HMAC-SHA256(UTF("")))
			//decode base64 encoded string HMAC-SHA256(UTF())
			//generate HMAC-SHA256(UTF()) using user key
			//compare recived hmac and genereated hmac
			std::pair<utility::string_t, utility::string_t > userandsignature = splitUserAndPassword();
			if (userandsignature.second == createSignature(utility::conversions::to_utf8string(userandsignature.first)))
				return true;
			return false;
		}

		std::pair<utility::string_t, utility::string_t > Signature::splitUserAndPassword()
		{
			utility::string_t::size_type pos = m_auth_string.find(':');
			if (m_auth_string.npos != pos) //if a valid signature
				return std::make_pair(m_auth_string.substr(0, pos), m_auth_string.substr(pos + 1)); //return account name, signature pair
			return std::make_pair(L"", L""); //return empty pair if invalid signature
		}

		utility::string_t Signature::createSignature(std::string username)
		{
			uint8_t hmacsha256digest[sha256::HASH_SIZE];
			utility::string_t val_sig = m_http_method + U("\n") + m_datetime + U("\n") + m_resource;

			User user;
			std::string usern = user.getkey(username);
			hmac<sha256>::calc(utility::conversions::to_utf8string(val_sig), usern, hmacsha256digest);
			//hmac<sha256>::calc(utility::conversions::to_utf8string(val_sig), "abcd123", hmacsha256digest);
			utility::string_t signa = utility::conversions::to_string_t(base64::encode_from_array(hmacsha256digest, 32));
			return signa;
		}

		utility::string_t Signature::getUsername() const
		{
			return m_username;
		}

		utility::string_t Signature::getSignature() const
		{
			return m_signature;
		}
	}
}