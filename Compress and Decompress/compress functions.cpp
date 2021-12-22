
class c_node {
	private:
		char char_;                          // char of the cnode
		c_node* left;                         // left child if any
		c_node* right;                        // right child  if any
		std::string huff_code;               //new huffmancode to assign to the char
							   // how many time this char is occupy in whole document
	public:
		int frequency;
		c_node(char char_);
		c_node(int freq);
		c_node();
		~c_node();
		friend class Fcompress;
		friend class Compare;

	}; typedef c_node* nptr;

	class Compare {                       //compare fn for min heap queue
	public:
		bool operator()(const c_node* x, const c_node* y)const
		{
			return (x->frequency > y->frequency);
		}
	};

	class Fcompress {

	private:
		//private attributes
		char char_;                          //buffer to read file
		c_node* code_table[128];             //table of code indexed with ascii index (0 --> 127)
		c_node* root;
		std::priority_queue<nptr, std::vector<nptr>, Compare> min_Q;
		std::string r_filename;
		std::fstream r_file;
		std::fstream w_file;
		string converted_output_path;
		// private methods
		void free_resources(c_node* p);
		void count_frq();                      // count frequencies of all charachters in document
		void create_tree();                                     // create huffman code tree using min heap (priority Q)
		void create_tree(std::string code, char char_);          // create tree from compressed file header
		void calculate_leaves(c_node* p, std::string code);
		void fill_table();                                      // fill generated huffman codes to table
		int Bin_t_Dec(std::string b);
		std::string Dec_t_Bin(int d);
		void write_to_file();
		void create_tree_from_header();
		void read_from_file();

	public:
		Fcompress(std::string rfilename);
		~Fcompress();
		void compress();
		void decompress();
		void save_as();
		void save_as_xml();




	};

	c_node::c_node(char char_)
	{
		this->char_ = char_;
		this->frequency = 0;
		this->left = NULL;
		this->right = NULL;
	}
	c_node::c_node(int freq)
	{
		this->frequency = freq;
		this->left = NULL;
		this->right = NULL;
	}
	c_node::c_node() {
		this->left = NULL;
		this->right = NULL;
	}
	c_node::~c_node() {

	}

///////////O(1)
	Fcompress::Fcompress(std::string rfilename) {
		for (int i = 0; i < 128; i++)
		{
			this->code_table[i] = new c_node((char)i);
		}
		this->r_filename = rfilename;
		//this->w_filename = wfilename;
	}
	Fcompress::~Fcompress() {
		free_resources(this->root);
	}

	void Fcompress::save_as() {
		SaveFileDialog^ saveFileDialog1 = gcnew SaveFileDialog;
		saveFileDialog1->InitialDirectory = "c:\\";
		saveFileDialog1->Filter = "Compressed File (.txt)|.txt";
		saveFileDialog1->FilterIndex = 1;
		saveFileDialog1->RestoreDirectory = true;
		saveFileDialog1->Title = "Choose Compressed File Location Destination";

		if (System::Windows::Forms::DialogResult::OK != saveFileDialog1->ShowDialog()) return;

		converted_output_path = msclr::interop::marshal_as< string >(saveFileDialog1->FileName);


	}

	void Fcompress::save_as_xml() {
		SaveFileDialog^ saveFileDialog1 = gcnew SaveFileDialog;
		saveFileDialog1->InitialDirectory = "c:\\";
		saveFileDialog1->Filter = "XML File (.xml)|.xml";
		saveFileDialog1->FilterIndex = 1;
		saveFileDialog1->RestoreDirectory = true;
		saveFileDialog1->Title = "Choose Decompressed File Location Destination";

		if (System::Windows::Forms::DialogResult::OK != saveFileDialog1->ShowDialog()) return;

		converted_output_path = msclr::interop::marshal_as< string >(saveFileDialog1->FileName);


	}
////////// O(log n)  n num of nodes in huffman tree

	void Fcompress::free_resources(c_node* p) {
		if (p)
		{
			if (p->left) {
				free_resources(p->left);
			}
			if (p->right)
			{
				free_resources(p->right);
			}
			delete p;
		}
	}
/////////////O(n)
	void Fcompress::count_frq() {
		r_file.open(r_filename, std::ios::in);


		do {
			r_file.get(char_);
			code_table[(int)char_]->frequency++;

		} while (!r_file.eof());
		r_file.close();

	}
///////////O(n)
	void Fcompress::create_tree(std::string code, char chara) {
		c_node* parent = this->root;
		for (unsigned int i = 0; i < code.size(); i++)
		{
			if (code[i] == '1')
			{
				if (parent->right == NULL)
					parent->right = new c_node;
				parent = parent->right;
			}
			else if (code[i] == '0')
			{
				if (parent->left == NULL)
					parent->left = new c_node;
				parent = parent->left;
			}
		}
		parent->char_ = chara;
	}
////////////O(n)
	void Fcompress::create_tree() {
		//creat min heap
		for (int i = 0; i < 128; i++)
		{
			if (code_table[i]->frequency)   //execluding non occupied characters from heap
				min_Q.push(code_table[i]);
		}
		// create the huffman tree
		std::priority_queue<nptr, std::vector<nptr>, Compare> temp(min_Q);
		while (temp.size() > 1) {
			root = new c_node;
			root->left = temp.top();
			temp.pop();
			root->right = temp.top();
			temp.pop();
			root->frequency = (root->left->frequency) + (root->right->frequency);
			temp.push(root);
		}
	}
/////////////
	void Fcompress::calculate_leaves(c_node* p, std::string code) {
		if (p->left == NULL && p->right == NULL)
		{
			p->huff_code = code;
		}
		else
		{
			calculate_leaves(p->left, code + '0');
			calculate_leaves(p->right, code + '1');
		}
	}

	void Fcompress::fill_table() {
		calculate_leaves(root, "");
	}
/////////O(n)
	int Fcompress::Bin_t_Dec(std::string b)
	{
		int n = b.length();
		int sum = 0;

		for (int i = n - 1; i >= 0; i--)
		{
			int temp = (int)(b[n - i - 1] - '0') * (pow(2, i));
			sum += temp;

		}
		return sum;
	}
/////////O(n)
	std::string Fcompress::Dec_t_Bin(int d)
	{
		std::string temp = "", B = "";
		while (d > 0) {
			temp = (char)(d % 2 + '0') + temp;
			d = d / 2;
		}
		B.assign(8 - temp.length(), '0');
		B.append(temp);
		return B;
	}
/////////O(n^2)
	void Fcompress::write_to_file() {
		std::string to_write = "";
		std::string code = "";
		to_write += (char)min_Q.size();
		std::priority_queue<nptr, std::vector<nptr>, Compare> temp(min_Q);

		while (!temp.empty())
		{  // Save table in header of compressed file
			c_node* node = temp.top();
			to_write += node->char_;
			code.assign(127 - node->huff_code.size(), '0');    //put 0's to fill 127 bit chunck
			code += '1';                              //put 1 just before the real code (help us when reading it)
			code.append(node->huff_code);
			do {
				to_write += (char)Bin_t_Dec(code.substr(0, 8));
				code = code.substr(8);
			} while (code.length());
			temp.pop();
		}
		code.clear();
		// compress file data
		r_file.open(r_filename, std::ios::in);
		w_file.open(converted_output_path, std::ios::out | std::ios::binary);

		//    do {
		//        r_file.get(char_);
		//        code+=code_table[(int)char_]->huff_code;
		//        // packing huffman code to file as bytes
		//        while (code.size()>8)
		//        {
		//            to_write+=(char)Bin_t_Dec(code.substr(0,8));
		//            code=code.substr(8);
		//        }

		//       }while (!r_file.eof());
		r_file.get(char_);
		while (!r_file.eof())
		{

			code += code_table[(int)char_]->huff_code;
			// packing huffman code to file as bytes
			while (code.size() > 8)
			{
				to_write += (char)Bin_t_Dec(code.substr(0, 8));
				code = code.substr(8);
			}
			r_file.get(char_);
		}
		int zeros = 8 - code.size();
		//    for (int i=0;i<zeros;i++)
		//    {
		//        code='0'+code;
		//    }

		to_write += (char)Bin_t_Dec(code);
		to_write += (char)zeros;


		w_file.write(to_write.c_str(), to_write.size());
		r_file.close();
		w_file.close();
	}
     ////////O(n^2)
	void Fcompress::create_tree_from_header() {
		r_file.open(r_filename, std::ios::in | std::ios::binary);
		unsigned char len;
		r_file.read(reinterpret_cast<char*>(&len), 1); // reinterpret_cast<char*> casting char to char* in cpp;
		root = new c_node;
		for (int i = 0; i < len; i++)
		{// every node saved as 1byte of orignal char and 16 byte of huff code
			char orig;                          //orignal byte
			unsigned char huffcode[16];         // huffman code to that orignal byte
			r_file.read(&orig, 1);
			r_file.read(reinterpret_cast<char*>(&huffcode), 16);
			std::string code = "";             // string representation of huffcode (i.e '10101')
			for (int j = 0; j < 16; j++)
			{
				code += Dec_t_Bin(huffcode[j]);
			}

			while (code[0] != '1')
			{//remove zerose added to 128 bit chunck till '1' we added before
				code = code.substr(1);
			}
			code = code.substr(1);// remove the extra '1' too
			create_tree(code, orig);
		}
		r_file.close();
	}
 ////////////// O(n^2)
	void Fcompress::read_from_file() {

		r_file.open(r_filename, std::ios::in | std::ios::binary);
		
		w_file.open(converted_output_path, std::ios::out);
		unsigned char len;          //length of nodes stored in header
		char zeros;                 //number of zeroes in last byte of file
		r_file.read(reinterpret_cast<char*>(&len), 1);
		r_file.seekg(-1, std::ios::end);  // seek to last byte in file that represent zeroes;
		int last = r_file.tellg();
		r_file.read(&zeros, 1);
		r_file.seekg(1 + (17 * len), std::ios::beg);    //seek to first byte of file encoded data




		std::vector<unsigned char> wholetext;
		unsigned char segment;
		r_file.read(reinterpret_cast<char*>(&segment), 1);
		while (!r_file.eof())
		{//get the text byte by byte using unsigned char
			wholetext.push_back(segment);
			r_file.read(reinterpret_cast<char*>(&segment), 1);
		}
		c_node* parent = root;
		std::string code;
                ////////////// O(n^2)
		for (unsigned int i = 0; i < wholetext.size() - 1; i++)
		{ 
                        //O(n)
			code = Dec_t_Bin(wholetext[i]);
			if (i == wholetext.size() - 2)
				code = code.substr(zeros);
			for (unsigned int j = 0; j < code.size(); j++)
			{
				if (code[j] == '0')
					parent = parent->left;
				else
					parent = parent->right;
				if (parent->left == NULL && parent->right == NULL)
				{
					w_file.put(parent->char_);
					parent = root;
				}
			}
		}
		r_file.close();
		w_file.close();


	}




///////////// O(n.m)
	void Fcompress::compress() {

		count_frq();
		create_tree();
		fill_table();
		write_to_file();
	}
///////////// O(n.m) 
	void Fcompress::decompress() {
		create_tree_from_header();
		read_from_file();
	}
