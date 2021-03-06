/*
	This file is part of Nanos6 and is licensed under the terms contained in the COPYING file.
	
	Copyright (C) 2015-2017 Barcelona Supercomputing Center (BSC)
*/

#ifndef DATA_ACCESS_SEQUENCE_LINKING_ARTIFACTS_IMPLEMENTATION_HPP
#define DATA_ACCESS_SEQUENCE_LINKING_ARTIFACTS_IMPLEMENTATION_HPP

#include <boost/intrusive/parent_from_member.hpp>

#include "DataAccessSequence.hpp"
#include "DataAccessSequenceLinkingArtifacts.hpp"


inline constexpr DataAccessSequenceLinkingArtifacts::hook_ptr DataAccessSequenceLinkingArtifacts::to_hook_ptr (DataAccessSequenceLinkingArtifacts::value_type &value)
{
	return &value._accessSequenceLinks;
}

inline constexpr DataAccessSequenceLinkingArtifacts::const_hook_ptr DataAccessSequenceLinkingArtifacts::to_hook_ptr(const DataAccessSequenceLinkingArtifacts::value_type &value)
{
	return &value._accessSequenceLinks;
}

inline DataAccessSequenceLinkingArtifacts::pointer DataAccessSequenceLinkingArtifacts::to_value_ptr(DataAccessSequenceLinkingArtifacts::hook_ptr n)
{
	return boost::intrusive::get_parent_from_member<DataAccess>(n, &DataAccess::_accessSequenceLinks);
}

inline DataAccessSequenceLinkingArtifacts::const_pointer DataAccessSequenceLinkingArtifacts::to_value_ptr(DataAccessSequenceLinkingArtifacts::const_hook_ptr n)
{
	return boost::intrusive::get_parent_from_member<DataAccess>(n, &DataAccess::_accessSequenceLinks);
}


#endif // DATA_ACCESS_SEQUENCE_LINKING_ARTIFACTS_IMPLEMENTATION_HPP
