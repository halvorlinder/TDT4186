## Task e)

The root of the security issue is that the GET requests can contain the string "../", which allows the requestor to request files outside the directory that contains the files that the server is supposed to be serving. 


### Solution 1:

One possible solution is to simply parse the path of the requested file. If the path contains the substring "../", the server could either respond with an error, or remove the substring from the path and try to fetch the file at the modified path. The prior of these options would probably make the most sense.


### Solution 2:

Lets assume a unix machine is running two webservers at once; website A and website B. The machine can then have two users A and B that only have read access to the html-files for their respective websites, as well as excecute access to the webserver binary and to the respective directories that contain the html-files. If the webservers are then started by user A and user B respectively (for example with the su command), the processes will then only be able to read the intended files. 


The most sensible thing might be to do both as they provide security on the application and the systemlevel respectively. 

