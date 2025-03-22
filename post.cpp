// Evan Hackstadt 2025 Winter CSCI 62

#include "post.h"
#include "network.h"

#include <string>
#include <vector>
#include <set>



// -- POST Class --

// Constructors

Post::Post() {
	messageId_ = 0;
	ownerId_ = 0;
	message_ = "";
	likes_ = 0;
}

Post::Post(int messageId, int ownerId, std::string message, int likes) {
	messageId_ = messageId;
	ownerId_ = ownerId;
	message_ = message;
	likes_ = likes;
}

// Accessors

std::string Post::toString() {
    if (getLikes() == 1) {
        return getMessage() + " Liked by " + std::to_string(getLikes()) + " person.";
    }
	return getMessage() + " Liked by " + std::to_string(getLikes()) + " people.";
}

int Post::getMessageId() { return messageId_; }

int Post::getOwnerId() { return ownerId_; }

std::string Post::getMessage() { return message_; }

int Post::getLikes() { return likes_; }

std::string Post::getAuthor() { return ""; }

bool Post::getIsPublic() { return true; }






// -- INCOMING POST Class --

// Constructors

IncomingPost::IncomingPost() : Post() {
	author_ = "";
	isPublic_ = true;
}

// Primary Constructor
IncomingPost::IncomingPost(int messageId, int ownerId, std::string message, int likes,
	bool isPublic, std::string author) : Post(messageId, ownerId, message, likes) {
	author_ = author;
	isPublic_ = isPublic;
}

// Accessors

std::string IncomingPost::toString() {
	return getAuthor() + " wrote"
	+ (getIsPublic() ? "" : " (private)")
	+ ": " + Post::toString();
}

std::string IncomingPost::getAuthor() { return author_; }

bool IncomingPost::getIsPublic() { return isPublic_; }




// -- POLL OPTION Helper Class --

// Constructors

PollOption::PollOption() {
	option_ = "";
	votes_ = 0;
	voters_ = std::set<int>();
}

PollOption::PollOption(std::string option) : PollOption() {
	option_ = option;
}

PollOption::PollOption(std::string option, int votes, std::set<int> voters) {
	option_ = option;
	votes_ = votes;
	voters_ = voters;
}

std::string PollOption::toString(int total) {
	return getOption() + " - "
	+ std::to_string(getVotesPercent(total)) + "%"
	+ " (" + std::to_string(getVotes()) + " votes)";
}

bool PollOption::hasVoted(int id) {
	for (int voter_id : getVoters()) {
		if (voter_id == id) {
			return true;
		}
	}
	return false;
}

std::string PollOption::getOption() { return option_; }

int PollOption::getVotes() { return votes_; }

int PollOption::getVotesPercent(int total) {
	return votes_ * 100 / total;
}

std::set<int> PollOption::getVoters() { return voters_; }

// Mutators

void PollOption::addVote(int voter_id) {
	voters_.insert(voter_id);
	votes_++;
}

void PollOption::removeVote(int voter_id) {
	if (hasVoted(voter_id)) {
		voters_.erase(voter_id);
		votes_--;
	}
}


// -- POLL Class --

// Constructors

Poll::Poll() {
	messageId_ = 0;
	ownerId_ = 0;
	message_ = "";
	options_ = std::vector<PollOption*>();
	totalVotes_ = 0;
}

Poll::Poll(int messageId, int ownerId, std::string message,
		std::vector<PollOption*> options, int totalVotes) {
	messageId_ = messageId;
	ownerId_ = ownerId;
	message_ = message;
	totalVotes_ = totalVotes;
	if (options.size() <= 10) {
		options_ = options;
	} else {
		for (int i=0; i<10; i++) {
			options_.push_back(options[i]);
		}
	}
}

// Accessors

std::string Poll::toString() {
	std::string output;
	output += getMessage();
	output += "\n";
	for (auto option : getOptions()) {
		output += option->toString(getTotalVotes());
		output += "\n";
	}
	return output;
}

int Poll::getMessageId() { return messageId_; }

int Poll::getOwnerId() { return ownerId_; }

std::string Poll::getMessage() { return message_; }

std::vector<PollOption*> Poll::getOptions() { return options_; }

PollOption * Poll::getOption(int i) { return options_[i]; }

int Poll::numOptions() { return options_.size(); }

int Poll::getTotalVotes() { return totalVotes_; }

// Mutators

void Poll::addOption(std::string option) {
	if (getOptions().size() < 10) {
		options_.push_back(new PollOption(option));
	}
}

void Poll::removeOption() {
	if (!getOptions().empty()) {
		options_.pop_back();
	}
}

void Poll::addVote(int voterId, int option) {
	// check that user has yet to vote
	for (auto option : getOptions()) {
		if (option->hasVoted(voterId)) {
			return;
		}
	}
	// cleared. delegate to Poll Option
	getOption(option)->addVote(voterId);
	totalVotes_++;
}

void Poll::removeVote(int voterId, int option) {
	if (getOption(option)->hasVoted(voterId)) {
		getOption(option)->removeVote(voterId);
		totalVotes_--;
	}
}