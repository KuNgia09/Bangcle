### About
Bangcle  is a Android Protector

It use  the  second generation Android Hardening Protection,can load the encrypted dex file from memory dynamically

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

but Bangcle don't support App which have rewrite Application currently

### How to use
1. use Apktool decompile the Apk what you need to protect
2. extract the **classes.dex** from Apk,rename **classes.dex** to jiami.dat,then place **jiami.dat** file to assets dir
3. copy **smali** dir files
4. modify **AndroidManifest.xml**
   add  **android:name="com.storm.fengyue.StubApplication"** under application node
5. use Apktool rebuild
