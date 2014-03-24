#include "stdafx.h"

#include "GitIndex.h"
#include "GitClient/GitAdd.h"

using namespace System;
using namespace SharpGit;
using namespace SharpGit::Implementation;
using namespace SharpGit::Plumbing;

struct git_index {};

void GitIndex::AssertOpen()
{
	if (! _index)
		throw gcnew InvalidOperationException();
}

bool GitIndex::Add(String ^relPath)
{
	AssertOpen();

	return Add(relPath, gcnew GitAddArgs());
}

bool GitIndex::Add(String ^relPath, GitAddArgs ^args)
{
	AssertOpen();

	GitPool pool;

	return Add(pool.AllocRelpath(relPath), args, %pool);
}

bool GitIndex::Add(const char *relPath, GitAddArgs ^args, GitPool ^pool)
{
	if (! relPath || !*relPath)
		throw gcnew ArgumentNullException("relPath");
	else if (! args)
		throw gcnew ArgumentNullException("args");
	AssertOpen();

	int r;

	// TODO: Use append2/index2 for advanced options
	
	if (args->Append)
		r = git_index_append(_index, relPath, args->Stage);
	else
		r = git_index_add(_index, relPath, args->Stage);

	return args->HandleGitError(this, r);
}

bool GitIndex::Contains(String ^relPath)
{
	AssertOpen();

	GitPool pool;

	return git_index_find(_index, pool.AllocRelpath(relPath)) >= 0;
}

bool GitIndex::Write()
{
	return Write(gcnew GitNoArgs());
}

bool GitIndex::Write(GitArgs ^args)
{
	if (! args)
		throw gcnew ArgumentNullException("args");

	AssertOpen();

	int r = git_index_write(_index);

	return args->HandleGitError(this, r);
}
