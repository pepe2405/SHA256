# SHA256

SHA-256 hashing algorithm, written in C++ 

## How does the program works:
1) The file with the string to be hashed <br />
  1.1 You can add your own file <br />
  1.2 You can edit the default "text.txt" file
2) You have to enter the name of the file ("text.txt")
3) On the console will be printed the hash value and it will be saved automaticly in "result.txt" file <br />
  NOTE: if the result.txt file is deleted the hash value will be printed on the console but not saved!
4) To be added: Printing all saved has values

## Example usage
```
Enter a file name:
text.txt
The has value of the string:
b94d27b9934d3e08a52e52d7da7dabfac484efe37a5380ee9088f7ace2efcde9
```


## Things to be added/changed
1) Function to check if a string is already saved in the results
2) Fixed overwriting the same string multiple times in the results file
3) setting a default "Press enter for default text file" 
