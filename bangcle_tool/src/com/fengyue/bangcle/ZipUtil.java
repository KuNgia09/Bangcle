package com.fengyue.bangcle;

import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.util.zip.ZipEntry;
import java.util.zip.ZipFile;

import sun.rmi.runtime.Log;



public class ZipUtil {
	
	public static int extract(String str, String str2, String str3) {
        System.out.println("ZipUtil.extract para zipPath:" + str);
        System.out.println("ZipUtil.extract para entryName:" + str2);
        System.out.println("ZipUtil.extract para filePath:" + str3);
        try {
            File file = new File(str3);
            if (file.exists()) {
                file.delete();
            }
            FileOutputStream fileOutputStream = new FileOutputStream(file);
            ZipFile zipFile = new ZipFile(new File(str));
            ZipEntry entry = zipFile.getEntry(str2);
            if (entry == null) {
            	System.out.println("ZipUtil.extract zip error:entryName not exist");
            }
            InputStream inputStream = zipFile.getInputStream(entry);
            byte[] bArr = new byte[1024];
            while (true) {
                int read = inputStream.read(bArr);
                if (read > 0) {
                    fileOutputStream.write(bArr, 0, read);
                } else {
                    fileOutputStream.close();
                    inputStream.close();
                    zipFile.close();
                    return 0;
                }
            }
        } catch (Exception e) {
        	System.out.println("ZipUtil.extract exception");
            if (e.getMessage() != null) {
            	System.out.println(e.getMessage());
            }
            System.out.println(e.toString());
            return 1;
        }
    }

}
