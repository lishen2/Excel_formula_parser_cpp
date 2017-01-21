#include <string>
#include <vector>

using std::string;
using std::vector;

// StrUtil class
class StrUtils
{
	public:
		/**Function used to trim empty character
		 * from begin and end of the string
		 * @param string& string to trim
		 * @return string& trimed tring, same object of parameter
		 */
		static string trim(string&);

		/**Find if the given string is peresented
		 * in the given array
		 * @param const int char* array length
		 * @param const char*[] char* array, used to store string ary
		 * @return -1 if not found, otherwise return index
		 */
		static int indexOf(vector<string>&, string&);
		static int indexOf(const int, const char*[], const char*);

    static bool is_number(const std::string& s);

	private:
		static bool isEmpChar(const char);

		//static const char m_empChar[];
		//static const int m_empLen;
}; //class strutil
