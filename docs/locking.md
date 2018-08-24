# Locking

## All features involved

Lock file
- lock settings
- model information
- models, import/edit/remove
- branding

Lock settings
- camera settings
- window settings
- navigation settings
- starting year

Restrict to current
- add/remove nars
- add/remove ers
- add/remove/edit er categories
- import nars
- import ers

Disable Navigation
- stuck frozen

Narrative Lock
- add/remove slides
- edit slides
- slide duration, transition, drag and drop, etc

ER Lock
- er dialog
- set position

Expiration date

## Locking pathways

The locking pathways are all different and really complicated. TODO: cleanup. Why? Locking touches everything. Everything was developed months apart (me learning/trying different thigns). Thus everyone has different perspectives. Model outliner can't see the app. I didn't want narrative/res to connect to root. etc. etc.

Dialogs are generally easier, since they don't need live connections.

ModelOutliner
root->lock => MainWindow::onLockChanged => ModelOutliner

Narrative
root->restrict => GroupSignals::restrictToCurrent() => NarrativeControl::onRestrictChanged

Resources
root->restrict => GroupSignals::restrictToCurrent() => ERControl::onRestrictChanged

ModelInformation (dialog), mw connect
LockDialog (dialog), self connecting
Camera settings, self connecting, modal dialog
Navigation settings, self connecting, modal dialog

## Passwords

Storing passwords as plain text is a bad idea. Someone can easily rip passwords from online archives w/o users knowing.

Storing hashed passwords is a slightly better idea. However, simple hashed passwords are easily cracked - brute forcing you can do ~1billion SHA256 per second (easily crack a 10 digit password). A rainbow table uses precomputed hashes to go even faster.

Salting - adding random salt makes passwords safer against table attacks.

Slowing down - picking a slow/expensive algorithm makes it harder to brute force.

https://crackstation.net/hashing-security.htm

We are currently using PBKDF2.

## Content Encryption

There is no encryption of the actual file. So someone could easily convert a locked file into an unlocked file given our source code. This is too much work though

## Serialization

Narrative (offset api)
Resource (offset api)
Settings (object api, postload)
