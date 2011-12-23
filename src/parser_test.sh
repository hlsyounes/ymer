#!/bin/bash

die() {
  echo $1
  exit 1
}

# Parse good model from stdin.
message=$(cat src/testdata/example.sm | HEAPCHECK=normal ./parser - 2>&1)
exit_status=$?
test ${exit_status} -eq 0 || die "expected 0 as exit status; got ${exit_status}"
test -z "$(echo "${message}" | grep 'syntax error')" \
    || die "did not expect \"syntax error\" in message; got \"${message}\""

# Parse bad model from stdin.
message=$(cat src/testdata/invalid.pm | HEAPCHECK=normal ./parser - 2>&1)
exit_status=$?
test ${exit_status} -eq 1 || die "expected 1 as exit status; got ${exit_status}"
test -n "$(echo "${message}" | grep '4:syntax error')" \
    || die "expected \"4:syntax error\" in message; got \"${message}\""
