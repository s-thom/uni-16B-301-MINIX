# 301 Assignment
## Inter-process Communication Subscription Server

TODO: Put something here. Or not, it doesn't matter

## How to: `git`
### Basic Commands

#### New Branch

`git checkout -b <branch name>`

Leave off the `-b` to just switch branches

#### Merge

`git checkout <branch to merge into>`

`git merge <branch you are merging> --no-ff`

The `--no-ff` makes commit network graphs look nicer. 

### Merge Conflicts

`git` will tell you which files had conflicts. Open those up, and find the area that looks like

```
<<<<<<< HEAD
some stuff
goes here
=======
other stuff
will be
right here
>>>>>>> name of branch you merged
```

The top part is the code in the original branch, The bottom part is the code from the branch you merged into it.

To resolve the merge conflict: edit the file (usually picking the top or the bottom, and deleting the other, although you can do anything), make sure to remove the lines `git` added in, save, and commit. Done!

Really, that's it. Merge conflicts aren't that scary after all.


