/*
 * Tests for the file-format guarantees the mission checkpoint system relies on.
 *
 * A checkpoint has to stay readable across engine updates, and the way it achieves that is
 * entirely down to three properties of the self-describing file handler:
 *
 *   - a field the reader asks for but the file does not contain yields a default, so a field
 *     added to the engine after the checkpoint was written costs nothing;
 *   - a field the file contains but the reader never asks for is ignored, so a field removed
 *     from the engine costs nothing either;
 *   - an unrecognised section is skipped rather than aborting the read.
 *
 * Plus nested arrays, which the checkpoint format needs for ships -> subsystems -> banks and
 * which the JSON handler did not originally support.
 *
 * If any of these regress, checkpoints silently stop surviving engine updates, which is
 * exactly the failure the format exists to prevent -- hence testing them directly rather than
 * through a whole mission.
 */

#include "cfile/cfile.h"
#include "pilotfile/JSONFileHandler.h"
#include "util/FSTestFixture.h"

#include <gtest/gtest.h>

#include <memory>

class CheckpointFileTest : public test::FSTestFixture {
  public:
	CheckpointFileTest() : test::FSTestFixture(INIT_CFILE) {}

  protected:
	static const char* TestFileName() { return "checkpoint_format_test.json"; }

	static CFILE* openForWrite()
	{
		return cfopen(TestFileName(), "wb", CF_TYPE_PLAYERS, false,
		              CF_LOCATION_ROOT_USER | CF_LOCATION_ROOT_GAME | CF_LOCATION_TYPE_ROOT);
	}

	static CFILE* openForRead()
	{
		return cfopen(TestFileName(), "rb", CF_TYPE_PLAYERS, false,
		              CF_LOCATION_ROOT_USER | CF_LOCATION_ROOT_GAME | CF_LOCATION_TYPE_ROOT);
	}

	void TearDown() override
	{
		cf_delete(TestFileName(), CF_TYPE_PLAYERS, CF_LOCATION_ROOT_USER | CF_LOCATION_TYPE_ROOT);

		FSTestFixture::TearDown();
	}
};

// A field the writer never wrote must read back as the caller's default rather than failing.
// This is what lets a newer engine read an older checkpoint.
TEST_F(CheckpointFileTest, MissingFieldYieldsDefault)
{
	{
		auto fp = openForWrite();
		ASSERT_NE(fp, nullptr);

		std::unique_ptr<pilot::FileHandler> handler(new pilot::JSONFileHandler(fp, false));
		handler->writeInt("present_int", 42);
		handler->writeFloat("present_float", 1.5f);
		handler->flush();
	}

	auto fp = openForRead();
	ASSERT_NE(fp, nullptr);

	std::unique_ptr<pilot::FileHandler> handler(new pilot::JSONFileHandler(fp, true));

	EXPECT_TRUE(handler->hasField("present_int"));
	EXPECT_FALSE(handler->hasField("absent_int"));

	EXPECT_EQ(handler->readIntOr("present_int", 7), 42);
	EXPECT_EQ(handler->readIntOr("absent_int", 7), 7);

	EXPECT_FLOAT_EQ(handler->readFloatOr("present_float", 9.0f), 1.5f);
	EXPECT_FLOAT_EQ(handler->readFloatOr("absent_float", 9.0f), 9.0f);

	EXPECT_EQ(handler->readStringOr("absent_string", "fallback"), SCP_string("fallback"));
	EXPECT_TRUE(handler->readBoolOr("absent_bool", true));
}

// A field the file carries but this build no longer reads must simply be ignored.  This is
// what lets an older engine -- or a build that dropped a field -- read a newer checkpoint.
TEST_F(CheckpointFileTest, UnreadFieldIsIgnored)
{
	{
		auto fp = openForWrite();
		ASSERT_NE(fp, nullptr);

		std::unique_ptr<pilot::FileHandler> handler(new pilot::JSONFileHandler(fp, false));
		handler->writeInt("wanted", 1);
		handler->writeInt("retired_field", 999);
		handler->writeString("another_retired_field", "junk");
		handler->flush();
	}

	auto fp = openForRead();
	ASSERT_NE(fp, nullptr);

	std::unique_ptr<pilot::FileHandler> handler(new pilot::JSONFileHandler(fp, true));

	// Reading only the field we still care about must work, and must not be thrown off by the
	// two it does not know about.
	EXPECT_EQ(handler->readIntOr("wanted", 0), 1);
}

// readFloat has to accept a whole number written without a decimal point, because a
// hand-edited checkpoint will contain "0" where the engine wrote "0.0".
TEST_F(CheckpointFileTest, IntegerReadsAsFloat)
{
	{
		auto fp = openForWrite();
		ASSERT_NE(fp, nullptr);

		std::unique_ptr<pilot::FileHandler> handler(new pilot::JSONFileHandler(fp, false));
		handler->writeInt("written_as_int", 3);
		handler->flush();
	}

	auto fp = openForRead();
	ASSERT_NE(fp, nullptr);

	std::unique_ptr<pilot::FileHandler> handler(new pilot::JSONFileHandler(fp, true));

	EXPECT_FLOAT_EQ(handler->readFloatOr("written_as_int", 0.0f), 3.0f);
}

// The checkpoint format nests arrays (ships contain subsystems, which contain weapon banks),
// which the JSON handler originally refused to do.
TEST_F(CheckpointFileTest, NestedArraysRoundTrip)
{
	const int outer_count = 3;
	const int inner_counts[outer_count] = {2, 0, 4};

	{
		auto fp = openForWrite();
		ASSERT_NE(fp, nullptr);

		std::unique_ptr<pilot::FileHandler> handler(new pilot::JSONFileHandler(fp, false));

		handler->startArrayWrite("outer", outer_count);
		for (int i = 0; i < outer_count; i++) {
			handler->startSectionWrite(Section::Unnamed);
			handler->writeInt("index", i);

			handler->startArrayWrite("inner", inner_counts[i]);
			for (int j = 0; j < inner_counts[i]; j++) {
				handler->startSectionWrite(Section::Unnamed);
				handler->writeInt("value", i * 100 + j);
				handler->endSectionWrite();
			}
			handler->endArrayWrite();

			// Written after the nested array to prove the outer element is still the current
			// object once the inner array has been closed.
			handler->writeInt("trailing", i * 10);

			handler->endSectionWrite();
		}
		handler->endArrayWrite();

		handler->flush();
	}

	auto fp = openForRead();
	ASSERT_NE(fp, nullptr);

	std::unique_ptr<pilot::FileHandler> handler(new pilot::JSONFileHandler(fp, true));

	auto outer = handler->startArrayRead("outer");
	ASSERT_EQ(outer, static_cast<size_t>(outer_count));

	for (size_t i = 0; i < outer; i++, handler->nextArraySection()) {
		EXPECT_EQ(handler->readIntOr("index", -1), static_cast<int>(i));

		auto inner = handler->startArrayRead("inner");
		EXPECT_EQ(inner, static_cast<size_t>(inner_counts[i]));

		for (size_t j = 0; j < inner; j++, handler->nextArraySection()) {
			EXPECT_EQ(handler->readIntOr("value", -1), static_cast<int>(i * 100 + j));
		}
		handler->endArrayRead();

		// The outer array must have resumed at the right element.
		EXPECT_EQ(handler->readIntOr("trailing", -1), static_cast<int>(i * 10));
	}
	handler->endArrayRead();
}

// A section written by a newer engine must be skipped rather than aborting the read, and the
// sections either side of it must still come through.
TEST_F(CheckpointFileTest, UnknownSectionIsSkipped)
{
	{
		auto fp = openForWrite();
		ASSERT_NE(fp, nullptr);

		std::unique_ptr<pilot::FileHandler> handler(new pilot::JSONFileHandler(fp, false));

		handler->beginWritingSections();

		handler->startSectionWrite(Section::CheckpointInfo);
		handler->writeString("slot", "alpha");
		handler->endSectionWrite();

		// Stands in for a section this build does not know about.
		handler->startSectionWrite(Section::Techroom);
		handler->writeInt("unknown_payload", 5);
		handler->endSectionWrite();

		handler->startSectionWrite(Section::CheckpointClock);
		handler->writeInt("mission_time", 1234);
		handler->endSectionWrite();

		handler->endWritingSections();
		handler->flush();
	}

	auto fp = openForRead();
	ASSERT_NE(fp, nullptr);

	std::unique_ptr<pilot::FileHandler> handler(new pilot::JSONFileHandler(fp, true));

	SCP_string slot;
	int mission_time = 0;
	int skipped = 0;

	handler->beginSectionRead();
	while (handler->hasMoreSections()) {
		auto section = handler->nextSection();

		switch (section) {
		case Section::CheckpointInfo:
			slot = handler->readStringOr("slot", "");
			break;

		case Section::CheckpointClock:
			mission_time = handler->readIntOr("mission_time", 0);
			break;

		default:
			// This is the branch that keeps a newer file readable.
			++skipped;
			break;
		}
	}
	handler->endSectionRead();

	EXPECT_EQ(slot, SCP_string("alpha"));
	EXPECT_EQ(mission_time, 1234);
	EXPECT_EQ(skipped, 1);
}
