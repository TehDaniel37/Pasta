
badge_name="coverage"

for i in $(seq 1 10)
do
    make test 2>&1 >/dev/null
done

coverage=$(make cov | grep TOTAL | awk {'print $4'} | cut -d '%' -f 1)
color="lightgrey"

if (( $coverage < 50 ))
then
    color="red"
elif (( $coverage < 70 ))
then
    color="orange"
elif (( $coverage < 80 ))
then
    color="yellow"
elif (( $coverage < 90 ))
then
    color="yellowgreen"
elif (( $coverage < 95 ))
then
    color="green"
else
    color="lightgreen"
fi

url=https://img.shields.io/badge/${badge_name}-${coverage}%25-${color}.svg

wget -q -O doc/coverage.svg $url
