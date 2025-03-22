// Evan Hackstadt 2025 Winter CSCI 62

#ifndef POST_H
#define POST_H

#include <string>
#include <vector>
#include <set>


/* --- POST class --- */

class Post {

public:

	// CONSTRUCTORS

	// Default Constructor
	// pre: none
	// post: initializes an empty Post object with default values
	Post();

	// Primary Constructor
	// pre: none
	// post: initializes a Post object with given values
	Post(int messageId, int ownerId, std::string message, int likes);

	// ACCESSORS

	// pre: none
	// post: returns a summary string containing the Post's message and stating
	//       how many people have liked it
	virtual std::string toString();

	// pre: none
	// post: returns messageId_ of the Post
	int getMessageId();

	// pre: none
	// post: returns ownerId_ of the Post
	int getOwnerId();

	// pre: none
	// post: returns message_ of the Post
	std::string getMessage();

	// pre: none
	// post: returns likes_ of the Post
	int getLikes();

	// pre: none
	// post: returns author_ if IncomingPost; empty string if just Post
	virtual std::string getAuthor();

	// pre: none
	// post: returns isPublic_ if IncomingPost; true if just Post
	virtual bool getIsPublic();

private:
	int messageId_;
	int ownerId_;
	std::string message_;
	int likes_;

};


/* --- INCOMING POST class --- */

class IncomingPost : public Post {

public:

	// CONSTRUCTORS

	// Default Constructor
	// pre: none
	// post: initializes an empty Incoming Post object; delegates to Post()
	IncomingPost();

	// Primary Constructor
	IncomingPost(int messageId, int ownerId, std::string message, int likes,
		bool isPublic, std::string author);

	// ACCESSORS

	// pre: none
	// post: returns a summary string containing the Post's author,
	//       whether or not it's private, the message, and its number of likes
	std::string toString();

	// pre: none
	// post: returns author_
	std::string getAuthor();

	// pre: none
	// post: returns isPublic_
	bool getIsPublic();

private:
	std::string author_;
	bool isPublic_;

};


/* --- POLL POST classes --- */

// HELPER class for Poll
class PollOption {
public:
	// CONSTRUCTORS

	// pre: none
	// post: initializes an empty Poll Option
	PollOption();

	// pre: none
	// post: initializes a Poll Option with given name
	//       and other fields default (delegates)
	PollOption(std::string name);

	// pre: none
	// post: initializes a Poll Option with the given values
	PollOption(std::string name, int votes,
		std::set<int> voters);

	// ACCESSORS

	// pre: post option exists
	// post: returns a summary string of this Post Option that contains
	//       the name of the option and its number and % of votes
	std::string toString(int total);

	// pre: none
	// post: returns true if id user has voted on this Post Option
	bool hasVoted(int id);

	// pre: none
	// post: returns option_
	std::string getOption();

	// pre: none
	// post: returns votes_ (the total on this option)
	int getVotes();

	// pre: give the totalVotes on the Post
	// post: returns an integer percent of this option's votes/total
	int getVotesPercent(int total);

	// pre: none
	// post: returns voters_
	std::set<int> getVoters();

	// MUTATORS

	// pre: user has not voted (checked by Poll)
	// post: increases votes_ by 1, and adds voter's id to voters_,
	void addVote(int voter_id);

	// pre: user has voted for this option
	// post: decreases votes_ by 1, and removes voter's id to voters_
	void removeVote(int voter_id);


private:
	std::string option_;
	int votes_;
	std::set<int> voters_;

	// INVARIANTS:
	// voters_ contains the User IDs of the users who have selected this option
};



// does not inherit from Post
class Poll {

public:

	// CONSTRUCTORS

	// Default Constructor
	// pre: none
	// post: initializes an empty Incoming Post object; delegates to Post()
	Poll();

	// Primary Constructor
	Poll(int messageId, int ownerId, std::string message,
		std::vector<PollOption*> options, int totalVotes);

	// ACCESSORS

	// pre: none
	// post: returns a summary string containing the Post's message,
	//       each of the options, and what % of the vote they received
	std::string toString();

	// pre: none
	// post: returns messageId_ of the Post
	int getMessageId();

	// pre: none
	// post: returns ownerId_ of the Post
	int getOwnerId();

	// pre: none
	// post: returns message_ of the Post
	std::string getMessage();

	// pre: none
	// post: returns options_
	std::vector<PollOption*> getOptions();

	// pre: poll option exists
	// post: returns a pointer to the specified poll option
	PollOption * getOption(int i);

	// pre: none
	// post: returns the size of options_
	int numOptions();

	// pre: none
	// post: returns totalVotes_
	int getTotalVotes();

	// MUTATORS

	// pre: options_.size() < 10
	// post: adds a new PostOption to options_
	void addOption(std::string name);

	// pre: options.size() > 0
	// post: removes the option at the back of options_
	void removeOption();

	// pre: option exists in options_
	// post: adds 1 to the votes of given option,
	//       if user has yet to vote on this Poll
	void addVote(int voterId, int option);

	// pre: option exists in options_
	// post: removes 1 to the votes of given option,
	//       if user has already voted and the option has nonzero votes
	void removeVote(int voterId, int option);

private:
	int messageId_;
	int ownerId_;
	std::string message_;
	std::vector<PollOption*> options_;
	int totalVotes_;

	// INVARIANTS:
	// options_ is an array of PollOption objects, numbered 0,1,...,9
	// the maximum number of options allowed is 10
	// totalVotes_ is the sum of all votes on the poll
	// users can only vote on one Poll Option
	// Note: Poll inherits likes_, but does not utilize them

};



#endif    // POST_H
