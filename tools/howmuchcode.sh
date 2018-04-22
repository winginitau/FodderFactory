 find . -type f -exec /bin/cat {} \; | wc -l
 find . -type f -name \*.c -exec /bin/cat {} \; | wc -l
 find . -type f -name \*.cpp -exec /bin/cat {} \; | wc -l
 find . -type f -name \*.h -exec /bin/cat {} \; | wc -l
