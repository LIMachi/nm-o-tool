#!/bin/zsh
echo "\033[2J\033[1;1H"
echo "\x1b[31;1mtests for validator, no arguments\x1b[0m";
for file in ./valid_binaries/*/*;
do;
  echo "\x1b[32;1m"$file"\x1b[0m";
  ../validator/validator $file;
done;
echo "\n\n\x1b[31;1minvalid file detection for validator, no arguments\x1b[0m"
for file in ./corrupted_binaries/*;
do;
  echo "\x1b[32;1m"$file"\x1b[0m";
  ../validator/validator $file;
done;
