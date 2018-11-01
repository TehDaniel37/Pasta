#!/usr/bin/env bash

cov_badge_name="coverage"
cov_badge_color="lightgrey"
test_badge_name="tests"
test_badge_color="brightgreen"
coverage=$(make -s cov | grep TOTAL | awk "{'print $4'}" | cut -d '%' -f 1)
tests_passed=0
tests_failed=0

while read -r line
do
    if [[ ! -z $line ]]
    then
        passed=$(echo "$line" | grep -o "[0-9]\\+ succeeded" | cut -d ' ' -f 1)
        failed=$(echo "$line" | grep -o "[0-9]\\+ failed" | cut -d ' ' -f 1)
        (( tests_passed+=passed ))
        (( tests_failed+=failed ))
    fi
done <<< "$(make -s test)"

if (( $tests_failed > 0 ))
then
    test_badge_color="red"
fi

if (( coverage < 50 ))
then
    cov_badge_color="red"
elif (( coverage < 70 ))
then
    cov_badge_color="orange"
elif (( coverage < 80 ))
then
    cov_badge_color="yellow"
elif (( coverage < 90 ))
then
    cov_badge_color="yellowgreen"
elif (( coverage < 95 ))
then
    cov_badge_color="green"
else
    cov_badge_color="brightgreen"
fi

cov_url=https://img.shields.io/badge/${cov_badge_name}-${coverage}%25-${cov_badge_color}.svg\?style=for-the-badge
test_url=https://img.shields.io/badge/${test_badge_name}-${tests_passed}%20passed%20${tests_failed}%20failed-${test_badge_color}.svg\?style=for-the-badge

wget -q -O doc/coverage.svg "$cov_url"
wget -q -O doc/tests.svg "$test_url"

