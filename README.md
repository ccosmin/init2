# Motivation
Passing signals to Docker container processes requires the EXEC form for CMD or ENTRYPOINT.
However, when doing that, since a shell is no longer involved environment variable substitution is no longer available.
Use `init2` to:
* launch a process directly and not have it as a child of the shell. This ensures that the process properly receives signals sent by Docker
* pass to the process any environment variable (substitution would normally not be possible given that the shell is not used). Just use as parameter the env var name prefixed with two underscores.

# Compile
```
mkdir build
cd build
cmake ..
make
```

# Launch test program
```
./init2 testsignal
```
Any signals sent to `init2` are now properly forwarded to `testsignal`

# Use in Dockerfile
```
CMD ["./init2", "echo", "__PATH"]
```
Process `echo` will be launched and receive as argument the *contents* of the `PATH` env var. Using `${PATH}` here would not expand to contents of the `PATH` env var since the shell is not used with the EXEC form.

Check [here](https://cremarenco.com/2024/01/18/init2.html) for a more detailed description of the problem.
