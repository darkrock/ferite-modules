#! /bin/sh

 msgfmt -o test.mo test.po

echo "You need to copy test.mo to your systems locale dir "
echo "or use bindtextdomain to point to the location of the compiled"
echo "Message catalog"