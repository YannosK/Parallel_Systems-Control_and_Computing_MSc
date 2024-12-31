- [x] column method does not produce similar results with sequentials even for small systems
- [x] row method does produce pretty similar results but the number it prints lies
- [x] both methods have best results with the code that produces segmentation faults.
    - [x] Maybe something wrong with the curly braces ---> _NO_
    - [x] Maybe need to add scope ---> _DID NOT WORK_
- [ ] run columns again
    - [x] with scope ---> Created segmentation faults
    - [ ] wihtout the paralle for in the first for loop

<br>

# Scripts

So far in the DI PCs:

- for the rows method
    - speedup only for "static". "static,1" has slowdown
    - for 1000 you only have slow down. From 2500 and going up you do have speedup.
    - slower one I tried is 60.000. There is a danger you drain out the memory for larger ones (above 70.000 it usually crashes)
- for the columns method
    - speedup for all static methods but best one is "static,1". Slowdown for dynamic and speedup for guided above 5000 iterations
    - for 1000 you only have slow down. From 2500 and going up you do have speedup for the best case "static,1", *but it is not guaranteed*
    - worked for 60.000 but took way too long (40 seconds for parallel and 70 for seq)

**Iterations**:
- starting at 2500
- doubling it every time, for 5 times
- this means 6 values and goes up to 40.000

```Python
starting = 2500
print(starting)
for i in range(5):
    starting *= 2
    print(starting)
```

**Scheduling**:
- Do it for static, dynamic, guided
- You have to play with the chunksize so I suppose for everything do something like none, 1, max = threads / iterations

**Threads**:
- To be compatible with alex try 1, 2, 3, 4, 8 threads

## Notes on it
- Not sure if I have to even do it for 1 thread

## List
- [x] functions to create directories and files
- [x] functions to clean directories and files - and relative option
- [ ] compute averages

<br>

# Results

So far you only have to keep the results of the simple code saved as 'linux_di_2024_12_31' and ignore the maxchunk

You have to figure out the following, with this order
1. [x] if the simple or complex methods are better
    - especially for rows method simple method is better
2. [x] if default and maxchunk are the same
    - Not the same and default is better, but very slightly. Maybe we don't need the maxchunk
3. [x] if you need so many different iterations
    - yes keep them
4. [ ] if rows method or columns method is better
    - columns method has a huge performance dip
5. [ ] if something needs to run again
    - [ ] run again for columns with no parallel code for the matrix initialization

What follows is a study of these

## 1. Simple vs Complex

Here is a table of who wins for each

|Method|Simple|Complex|
|---|---|---|
|ROWS static 1|_X_||
|ROWS static d|_X_||
|ROWS dynamic 1|_X_||
|ROWS dynamic d|_X_||
|ROWS guided 1|_X_||
|ROWS guided d|_X_||
|COLUMNS static 1||_X_|
|COLUMNS static d|-|-|
|COLUMNS dynamic 1|_X_||
|COLUMNS dynamic d|_X_||
|COLUMNS guided 1|_X_||
|COLUMNS guided d|-|-|


<br>

## 2. Default vs Maxchunk

Default and maxchunk seem to not be the same. 

Here is a table of who wins for simple code

|Method|Default|Maxchunk|
|---|---|---|
|ROWS static|X||
|ROWS dynamic|_X_||
|ROWS guided|X||
|COLUMNS static||_X_|
|COLUMNS dynamic||X|
|COLUMNS guided|X||

Here is a table of who wins for the complex code

|Method|Default|Maxchunk|
|---|---|---|
|ROWS static|_X_||
|ROWS dynamic|-|-|
|ROWS guided|X||
|COLUMNS static|X||
|COLUMNS dynamic|_X_||
|COLUMNS guided|X||

<br>

## 3. Progression with iterations

YES means there is, NO means there is not.

|Method|YES|NO|
|---|---|---|
|ROWS static 1|X||
|ROWS static d|_X_||
|ROWS dynamic 1|_X_||
|ROWS dynamic d|X||
|ROWS guided 1|X||
|ROWS guided d|X||
|COLUMNS static 1||X*|
|COLUMNS static d||X*|
|COLUMNS dynamic 1||X**|
|COLUMNS dynamic d||X*|
|COLUMNS guided 1||X*|
|COLUMNS guided d||X**|

*There is regression after 1000!!!!
**There is regression and stabilization after 1000!!!!

<br>

# Latex tables

- leave efficiency out - do onlu speedup