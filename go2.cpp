#include <iostream>
#include <vector>

using namespace std;

void genSubseqs(string s, string out, vector<string>& vec)
{
	if (s.empty()) {
		if (!out.empty())
			vec.push_back(out);
		return;
	}

	genSubseqs(s.substr(1), out + s[0], vec);
	genSubseqs(s.substr(1), out, vec);
}

int main(void)
{
	string family = "ABCD";
	vector<string> subseqs;

	// generate subsequences
	genSubseqs(family, "", subseqs);

	// apply rules
	for (string& s : subseqs) {
		if (s.find('A') != string::npos && s.find('D') != string::npos)
			continue;

		if (s.find('B') != string::npos && s.find('C') == string::npos)
			continue;

		if ((s.find('A') != string::npos && s.find('C') != string::npos) &&
			s.find('B') != string::npos)
			continue;

		if (s.find('A') == string::npos && s.find('C') != string::npos &&
			s.find('D') != string::npos)
			continue;

		cout << s << " " << endl;
	}
}

