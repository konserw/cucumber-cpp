#!/bin/sh
# Reformat all code changed since this branch forked from the default branch
git fetch origin HEAD
if [ "${TRAVIS_PULL_REQUEST:-false}" = "false" ]; then
    BASE_HEAD="$(git rev-parse FETCH_HEAD)"
else
    BASE_HEAD="$(git merge-base FETCH_HEAD HEAD)"
fi
git clang-format-3.8 --binary=/usr/bin/clang-format-3.8 --style=file --commit="${BASE_HEAD}"
# Assert that all changes adhere to the asked for style
exec git diff --exit-code
