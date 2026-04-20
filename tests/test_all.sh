TEST_FOLDER=build 
# test if folder exists 
if [ ! -d "$TEST_FOLDER" ]; then 
	echo "$TEST_FOLDER does not exist. Aborting." 
	exit 1 
fi 

for filename in $TEST_FOLDER/*; do 
	"$filename" "$@" 
done