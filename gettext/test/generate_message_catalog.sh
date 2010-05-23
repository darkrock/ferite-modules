#! /bin/sh

echo "Generating message catlog from test.fe"
xgettext -k_ -kGettext\.gettext -o test.po -C test.fe

echo "Translate the generated file test.po and run the script "
echo "install_message_catalog.sh"