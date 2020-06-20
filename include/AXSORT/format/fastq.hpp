/**
 * @file vcf.hpp
 * @brief a parser of vcf format file
 *
 * @author JHH corp
 */

#pragma once

#include <exception>
#include <iostream>
#include <iterator>
#include <random>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>
//#include <Biovoltron/base_vector.hpp>

namespace biovoltron::format{

/**
 * @class FastqException
 * @brief An exception class which inherit std::runtime_error
 *
 * This class will be thrown if actions which belongs to VCF have 
 * exceptions.
 */
class FastqException : public std::runtime_error
{
  public:
    /**
     * @brief Message assign constructor
     *
     * @param m Message string wanted to be assigned
     */
    FastqException (const std::string& msg)
	: runtime_error(std::string("FastqException " + msg))
    {
    }
};

/**
 * @class Fastq
 * @brief A Fastq object which stores a line of fastq format, that is 
 * an entry of fastq data.
 * 
 * Member variable:<br>
 *	State state_<br>
 *	std::string name<br>
 *	NTable n_table<br>
 *	Sequence seq<br>
 *	QualType seq_qual<br>
 *
 * This class can store an entry of fastq data, and also provides 
 * entry parsing, access, modify and entry processing functions, 
 * like substr(), trim(), get_antisence().<br>
 * And also provides sequence compression by using 
 * biovoltron::base_vector, so that the size of sequence can be 
 * decressed to 1/4.
 */
template <typename Sequence = std::string, typename QualType = std::string>
class Fastq
{
    /**
     * @brief Simulate a table by using 
     * vector<pair<long int, long int>>
     *
     * NTable stores start point and length of every continueous 
     * n-base sequences
     */
    using NTable = std::vector<std::pair<size_t, size_t>>;

    /**
     * @enum state
     * @brief An enum class which indicates different state during 
     * parsing fastq files.
     *
     * This enum contains name, seq, plus, qual, end. The last one 
     * "end" is used for distinguish the end of a round. And others 
     * indicate different categories of lines in fastq format.
     */
    enum class State {name, seq, plus, qual, end};

    ///An State variable which is initialized to State::name
    State state_ = State::name;

  public:

	using string_type = Sequence;
	using qual_type = QualType;
	using n_table_type = NTable;

    ///A string which store the name line of a fastq entry
    std::string name;

    /**
     * A NTable which store the start point and length of all 
     * continuous n-base sub-sequence. Note that, after recording
	 * n-bases and corresponding seq_qual will be erased.
     */
    NTable n_table;

	/**
     * A "Sequence" which store sequence with "n" erased. Sequence 
	 * is template parameter
	 */
    Sequence seq;

    /**
	 * A QualType variable which store the quality line of a fastq 
	 * entry, Note that, quality charactor of n-bases are erased.
	 */
	QualType seq_qual;

    /**
     * @brief >> operator which get entry from an istream
     * 
     * @param is An istream where to get entry
     * @param fq A Fastq object to store gotten entry
     * 
     * @return Is identical to parameter is
     *
     * Just call Fastq::get_obj() to help parse given istream, and 
     * store them in given Fastq object.<br>
     * Should have the some property to get_obj function.
     *
     * @sa Fastq::get_obj()
     */
    friend std::istream& operator>>(std::istream& is, Fastq& fq)
    {
		return get_obj(is, fq);
    }

    /**
     * @brief << operator which write entry to an ostream
     * 
     * @param os An ostream where to write entry
     * @param fq A Fastq object which is going to write to os
     * 
     * @return Is identical to parameter os
     *
     * Just call Fastq::to_string() to help change data of given Fastq  
     * object to string then output to given ostream, and store them 
     * in given Fastq object.<br>
     * Should have the some property to Fastq::to_string()
     *
     * @sa Fastq::to_string()
     */
    friend std::ostream& operator<<(std::ostream& os, const Fastq& fq)
    {
		os << fq.to_string();
		return os;
    }

    /**
     * @brief Parse fastq entry from specific istream
     *
     * @param is An istream contains entry data
     * @param fq A Fastq object used to store parsed entry data
	 * @param erase_n_base_quality A bool to determine whether erase 
	 * seq_qual of n-base. The default value is true, if false is 
	 * specific that there is no length equality guaranteed between 
	 * seq length and seq_qual length
     * @return An istream identical to parameter is
     *
     * Use std::getline to get lines from istream and store in fq. 
     * Because there are 4 types of line for each fastq entry, use 
     * for loop to iterate state_ and use switch to compare states. 
     * If the format is correct, then just store data to respective 
     * variable. Otherwise, throw FastqException with the reason to 
     * warn user.<br>
     * Notice that, if there are n-bases in sequence. we store start 
     * point and length of every continueous n-base sub-sequence in 
     * n_table, then erase them, corresponding seq_qual are also 
	 * erased.
     *
     * Time complexity: O(n)<br>
     *	    \e n: the length of sequence<br>
     */
    static std::istream& get_obj(std::istream& is, Fastq& fq, bool erase_n_base_quality = true)
    {
		std::string buf;

		for (fq.state_ = State::name; 
			fq.state_ != State::end; 
			fq.state_ = (State)((size_t)fq.state_ + 1))
		{
			switch (fq.state_)
			{
				case State::name:
					if (!is.good())
						return is;

					std::getline(is, fq.name, '\n');

					if (fq.name.size() > 0 && fq.name.front() == '@')
						fq.name.erase(fq.name.begin());
					else
					{
						throw FastqException(
							"ERROR: get_obj(): Can't find \'@\' from input when state is equal "
							"to name\n"
						);
					}

					break;
				case State::seq:
					fq.seq.clear();

					if (!is.good())
					{
						throw FastqException(
							"ERROR: get_obj(): seq field of an entry is disappear\n"
						);
					}

					std::getline(is, buf, '\n');
					fq.seq.reserve(buf.size());

					for (auto it(buf.cbegin()); it != buf.cend(); ++it)
					{
						switch (*it)
						{
							case 'A':
							case 'a':
								fq.seq.push_back('A');
								break;
							case 'C':
							case 'c':
								fq.seq.push_back('C');
								break;
							case 'G':
							case 'g':
								fq.seq.push_back('G');
								break;
							case 'T':
							case 't':
								fq.seq.push_back('T');
								break;
							case 'N':
							case 'n':
							{
								fq.n_table.emplace_back(fq.seq.size(), 0);

								auto it2(it + 1);
								for (; it2 != buf.cend() && (*it2 == 'N' || *it2 == 'n'); ++it2);
								
								fq.n_table.back().second = std::distance(it, it2);
								it = it2 - 1;

								break;
							}
							default:
								throw FastqException(
									"ERROR: get_obj(): invalid input seq charactor\n"
								);
						}
					}

					break;
				case State::plus:
					if (!is.good())
					{
						throw FastqException(
							"ERROR: get_obj(): + field of an entry is disappear\n"
						);
					}

					std::getline(is, buf, '\n');

					if (buf.size() == 0 || buf.at(0) != '+')
					{
						throw FastqException(
							"ERROR: get_obj(): There is not \'+\' after seq line\n"
						);
					}
					else
					{
						if (buf.size() > 1 && fq.name != buf.substr(1))
							throw FastqException(
								"ERROR: get_obj(): There is string after \'+\' but not equal to "
								"string after \'@\'\n"
							);
					}

					break;
				case State::qual:
				{
					if (!is.good())
					{
						throw FastqException(
							"ERROR: get_obj(): seq_qual of an entry is disappear\n"
						);
					}

					std::getline(is, buf);
					fq.seq_qual.reserve(buf.size());

					if (erase_n_base_quality)
					{
						auto n_it(fq.n_table.cbegin());
						for (size_t i(0); i < buf.size(); ++i)
						{
							if (n_it == fq.n_table.cend() || i != n_it->first)
								fq.seq_qual.push_back(buf.at(i));
							else
							{
								i += n_it->second - 1;
								++n_it;
							}
						}
					}
					else
						fq.seq_qual = buf;

					if (erase_n_base_quality && fq.seq_qual.size() != fq.seq.size())
					{
						throw FastqException(
							"ERROR: get_obj(): length of seq_qual is different to length of seq\n"
						);
					}

					for (const auto base : fq.seq_qual)
					{
						if (base > '~' || base < '!')
							throw FastqException(
								"ERROR: get_obj(): wrong charactor in quality string\n"
							);
					}
					
					break;
				}
				default:
					break;
			}
		}

		return is;
    }

    /**
     * @brief Parse fastq entry from specific container which stores 
	 * 4 lines of fastq entry in 4 strings continuous
     *
     * @param is An container iterator points to the first line(aka 
	 * name line of a fastq entry)
	 * @param erase_n_base_quality A bool to determine whether erase 
	 * seq_qual of n-base. The default value is true, if false is 
	 * specific that there is no length equality guaranteed between 
	 * seq length and seq_qual length
     * @return The fastq entry required
     *
     * This function move second line and copy other 3 lines of 
	 * string from iterator to iterator + 3.<br>
     * Because there are 4 types of line for each fastq entry, use 
     * for loop to iterate state_ and use switch to compare states. 
     * If the format is correct, then just store data to respective 
     * variable. Otherwise, throw FastqException with the reason to 
     * warn user.<br>
     * Notice that, if there are n-bases in sequence. we store start 
     * point and length of every continueous n-base sub-sequence in 
     * n_table, then erase them, corresponding seq_qual are also 
	 * erased.
     *
     * Time complexity: O(n)<br>
     *	    \e n: the length of sequence<br>
     */
	template <typename Iterator>
	static Fastq parse_obj(Iterator it, bool erase_n_base_quality = true)
	{
		Fastq fq;
		for (fq.state_ = State::name; 
			fq.state_ != State::end; 
			fq.state_ = (State)((size_t)fq.state_ + 1), ++it)
		{
			switch (fq.state_)
			{
				case State::name:
					fq.name = std::move(*it);

					if (fq.name.size() > 0 && fq.name.front() == '@')
						fq.name.erase(fq.name.begin());
					else
						throw FastqException(
							"ERROR: get_obj(): format of name field is invalid\n"
						);

					break;
				case State::seq:
					fq.seq.reserve(it->size());

					for (auto it1(it->cbegin()); it1 != it->cend(); ++it1)
					{
						switch (*it1)
						{
							case 'A':
							case 'a':
								fq.seq.push_back('A');
								break;
							case 'C':
							case 'c':
								fq.seq.push_back('C');
								break;
							case 'G':
							case 'g':
								fq.seq.push_back('G');
								break;
							case 'T':
							case 't':
								fq.seq.push_back('T');
								break;
							case 'N':
							case 'n':
							{
								fq.n_table.emplace_back(fq.seq.size(), 0);

								auto it2(it1 + 1);
								for (; it2 != it->cend() && (*it2 == 'N' || *it2 == 'n'); ++it2);
								
								fq.n_table.back().second = std::distance(it1, it2);
								it1 = it2 - 1;

								break;
							}
							default:
								throw FastqException(
									"ERROR: get_obj(): invalid input seq charactor\n"
								);
						}
					}

					break;
				case State::plus:
					if (it->size() == 0 || it->at(0) != '+')
					{
						throw FastqException(
							"ERROR: get_obj(): There is not \'+\' after seq line\n"
						);
					}
					else
					{
						if (it->size() > 1 && fq.name != it->substr(1))
							throw FastqException(
								"ERROR: get_obj(): There is string after \'+\' but not "
								"equal to string after \'@\'\n"
							);
					}

					break;
				case State::qual:
					fq.seq_qual.reserve(it->size());

					if (erase_n_base_quality)
					{
						auto n_it(fq.n_table.cbegin());
						for (size_t i(0); i < it->size(); ++i)
						{
							if (n_it == fq.n_table.cend() || i != n_it->first)
								fq.seq_qual.push_back(it->at(i));
							else
							{
								i += n_it->second - 1;
								++n_it;
							}
						}
					}
					else
						fq.seq_qual = *it;

					if (erase_n_base_quality && fq.seq_qual.size() != fq.seq.size())
					{
						throw FastqException(
							"ERROR: get_obj(): length of seq_qual field is different to "
							"length of seq field\n"
						);
					}

					for (const auto i : fq.seq_qual)
					{
						if (i > '~' || i < '!')
							throw FastqException(
								"ERROR: get_obj(): wrong charactor in quality string\n"
							);
					}
			}
		}

		return fq;
    }

    /**
     * @brief Can get data of this Fastq object
     *
     * @return A string of data in fastq format
     *
     * Concatenate string of 4 lines by order, and if there are data 
     * in n_table, add those sub-sequence back to n-bases. But becuse 
	 * did not record quality charactor of those n-bases, they'll be 
	 * replaced to lowest quality charactor, that is "!".
     *
     * Time Complexity: O(n)<br>
     *	    \e n is the length of sequence
     */
    std::string to_string() const
    {
		std::string buf("@");
		std::string buf_qual;

		buf.append(name);
		buf.push_back('\n');

		auto n_it(n_table.cbegin());

		for (size_t i(0); i < seq.size(); ++i)
		{
			if (n_it == n_table.cend() || i != n_it->first)
			{
				buf.push_back(seq.at(i));
				buf_qual.push_back(seq_qual.at(i));
			}
			else
			{
				for (size_t j(0); j < n_it->second; ++j)
				{
					buf.push_back('N');
					buf_qual.push_back('!');
				}
			}
		}

		buf.append("\n+\n");
		buf.append(buf_qual);

		return buf;
    }

    /**
     * @brief Convert vector of Fastq to string then dump to an 
     * ostream
     *
     * @param os An ostream to dump fastq format data
     * @param v_fastq A vector<Fastq>, which we want to dump them to 
     * fastq format
     *
     * Convert vector<Fastq> to string by using range-for to append 
     * each fastq string which generated by Fastq::to_string() to a 
     * buffer. And write the buffer to os at the end.
     *
     * Time complexity: O(m * n)<br>
     *	    \e m: size of v_fastq
     *	    \e n: average length of seq of Fastq objects
     *
     * @sa Fastq::to_string()
     */
    static void dump(std::ostream& os, const std::vector<Fastq>& v_fastq)
    {
		std::string buf("");

		for (const auto& i : v_fastq)
		{
			buf.append(i.to_string());
			buf.append("\n");
		}

		buf.pop_back();
		os << buf;
    }

	/**
	 * @brief return size of seq and seq_qual
	 *
	 * @return size of seq and seq_qual
	 *
	 * Return size of seq and seq_qual. There is no length equality 
	 * guaranteed between seq length and seq_qual length.
	 *
	 * Time complexity: O(1)
	 */
	size_t size() const
	{
		return seq.size();
	}

    /**
     * @brief return a Fastq object with the same name but have 
     * specified sub-sequence and sub-sequence_quality
     *
     * @param pos The start point of specified sub-sequence
     * @param count The length of specified sub-sequence, if (pos + 
     * count) is greater than size of sequence, count will change to 
     * (size of sequence - pos) automatically
     * @return A Fastq object with the same name like original one, 
     * but has sub-sequence and sub-sequence_quality
     *
     * Generate a Fastq object with the same name like origin, but
     * sub-sequence and sub-sequence_quality are the specific part
     * of origin.<br>
     * Also modify n_table if there are continueous 
     * n-base sub-sequence are cut off.
     *
     * Time complexity: O(n)<br>
     *	    \e n: size of n_table
     */
    Fastq substr(size_t pos, size_t count = -1) const
    {
		if (pos >= seq.size())
			throw FastqException("ERROR: substr(): out_of_range_exception\n");

		Fastq tmp;
		const auto& fq_n(n_table);
		size_t n_end, begin, end;

		count = std::min(count, seq.size() - pos);

		tmp.name = name;
		tmp.seq = {seq.begin() + pos, seq.begin() + pos + count};
		tmp.seq_qual = seq_qual.substr(pos, count);
		tmp.n_table.reserve(fq_n.size());

		for (size_t i(0); i < fq_n.size(); ++i)
		{
			n_end = fq_n.at(i).first + fq_n.at(i).second;

			if (pos < fq_n.at(i).first)	
				begin = fq_n.at(i).first - pos;
			else if (pos >= n_end)
				continue;
			else
				begin = 0;

			if (pos + count < fq_n.at(i).first)
				break;
			else if (pos + count >= n_end)
				end = n_end - pos;
			else
				end = count;

			if (begin < end)
				tmp.n_table.emplace_back(begin, end - begin);
		}

		return tmp;
    }

    /**
     * @brief cut off sequence and sequence_quality after pos
     *
     * @param pos The start point of the part you want to cut off
     *
     * Cut off tail-end of sequence and sequence_quality, only 
     * reserve those parts before pos. Also modified 
     * n_table to make sure its make sence.
     *
     * Time complexity: O(n)<br>
     *	    \e n: size of n_table
     *
     * @sa Fastq::substr()
     */
    void trim(size_t pos)
    {
		if (pos > seq.size())
			throw FastqException("ERROR: trim(): out_of_range_exception\n");

		auto& fq_n(n_table);
		size_t n_end;

		seq.resize(pos);
		seq_qual.resize(pos);

		for (size_t i(0); i < fq_n.size(); ++i)
		{
			n_end = fq_n.at(i).first + fq_n.at(i).second;
			
			if (n_end <= pos)
				continue;
			else if (n_end > pos && fq_n.at(i).first < pos)
				fq_n.at(i).second = pos - fq_n.at(i).first;
			else
			{
				fq_n.resize(i);
				break;
			}
		}
    }

	/**
	 * @brief a forward declarion of "has_flip" struct
	 */
	template <typename, typename T>
	struct has_flip;

	/**
	 * @brief a helper struct which can determine if an object has 
	 * member function "flip()" at compile time
	 *
	 * @tparam Sequence_ The type which need to be determined if it 
	 * has member function "flip()"
	 * @tparam FuncRet Expected return type of function "flip()"
	 * @tparam FuncArgs Expected parameter pack types of function 
	 * "flip()"
	 *
	 * A helper struct which can determine if an object has member 
	 * function "flip()" with expected return type and parameter pack 
	 * types.<br>
	 * This struct try to construct parameters and sequence whose 
	 * type are FuncArgs and Sequence_, respectively. After 
	 * determined if the return type is the same as FuncRet. Store a 
	 * bool value in member variable "value" to represent if 
	 * Sequence_ has member funtion "flip()".<br>
	 * Note that, all determinations are done at compile time, and 
	 * member variable "value" is constexpr, thus this struct could 
	 * be used in SFINAE or constexpr if.
	 *
	 * Time Complexity: <compile time>
	 */
	template <typename Sequence_, typename FuncRet, typename... FuncArgs>
	struct has_flip<Sequence_, FuncRet(FuncArgs...)>
	{
	  private:
		template <typename>
		static constexpr std::false_type check(...);

		template <typename T>
		static constexpr auto check(T*) 
			-> typename std::is_same<
					decltype(std::declval<T>().flip(std::declval<FuncArgs>()...)), 
					FuncRet
				>::type;

		using bool_type = decltype(check<Sequence_>(0));

	  public:
		static constexpr bool value{bool_type::value};	
	};

    /**
     * @brief return reverse complement of sequence, only enable if 
     * template parameter "Sequence" of Fastq object has member 
	 * function "flip()"
     *
     * @return A sequence which is reverse complement of origin
     *
     * Generate an object with r_iterator to get reverse sequence 
	 * then use the member function "flip" defined in Sequence type 
     * to complement every base.
     *
     * Time Complexity: O(n)<br>
     *	    \e n: length of sequence
     */
	template<
		typename T = Sequence, 
		typename std::enable_if_t<has_flip<T, void(void)>::value, int> = 0
	>
	Sequence get_rc_seq() const noexcept
	{
		Sequence s(seq.rbegin(), seq.rend());
		s.flip();

		return s;
	}

    /**
     * @brief return reverse complement of sequence, only enable if 
     * template parameter "Sequence" of Fastq object does not has 
	 * member function "flip()"
     *
     * @return A sequence which is reverse complement of origin
	 * 
     * Generate an object with r_iterator to get reverse sequence 
	 * then using for loop to complement bases one by one.
     *
     * Time Complexity: O(n)<br>
     *	    \e n: length of sequence
     */
	template<
		typename T = Sequence, 
		typename std::enable_if_t<!has_flip<T, void(void)>::value, int> = 0
	>
	Sequence get_rc_seq() const noexcept
	{
		Sequence s(seq.rbegin(), seq.rend());

		for (auto& i : s)
		{
			switch(i)
			{
			case 'A':
				i = 'T';
				break;
			case 'C':
				i = 'G';
				break;
			case 'G':
				i = 'C';
				break;
			case 'T':
				i = 'A';
				break;
			}
		}

		return s;
	}
};

}

