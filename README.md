### About
Bangcle  is a Android Protector

It use  the  second generation Android Hardening Protection, load the encrypted dex file from memory dynamically

### Compatibility
Support Android Version
- 4.4
- 5.0
- 5.1
- 6.0
- 7.0
- 7.1
- 8.0
- 8.1

but Bangcle don't support Apk which have rewrite Application currently.

### How to use
1. use Apktool decompile the Apk what you need to protect
2. extract the **classes.dex** from Apk,rename **classes.dex** to jiami.dat,then place **jiami.dat** file to assets dir
3. cd **jni** dir, exec **ndk-build**,copy the generated **libdexload.so** to **lib** dir
4. copy **smali** dir files
5. modify **AndroidManifest.xml**
   add  **android:name="com.storm.fengyue.StubApplication"** under application node
6. use Apktool rebuild

of course,you can write a script to achieve the above steps automatically
