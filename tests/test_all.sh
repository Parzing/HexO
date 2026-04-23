TEST_FOLDER=build 
# test if folder exists 
if [ ! -d "$TEST_FOLDER" ]; then 
	echo "$TEST_FOLDER does not exist. Aborting." 
	exit 1 
fi 

OUTPUT_FILE="test_results.txt"


for filename in $TEST_FOLDER/*; do 
	"$filename" "$@" >> "${OUTPUT_FILE}"
done

TOTAL_PASS=0
TOTAL_TESTS=0


# basically grep pipes the matches to read, which uses the IFS as a delimiter
# so grep passes ab/cd to read, which uses / as delimiter, which puts those 
# into PASS & TESTS
while IFS='/' read -r PASS TESTS; do
  TOTAL_PASS=$((TOTAL_PASS + PASS))
  TOTAL_TESTS=$((TOTAL_TESTS + TESTS))
done < <(grep -oE '[0-9]+/[0-9]+' "$OUTPUT_FILE")

echo -e "\nSummary: ${TOTAL_PASS} tests passed out of ${TOTAL_TESTS}." >> "${OUTPUT_FILE}"

cat ${OUTPUT_FILE}

rm ${OUTPUT_FILE}
