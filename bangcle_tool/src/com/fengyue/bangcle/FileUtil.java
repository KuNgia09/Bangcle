package com.fengyue.bangcle;

import java.io.BufferedOutputStream;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.List;

public class FileUtil {

	public static void delete(File file){
		if(file!=null&&file.exists()){
			if(file.isDirectory()){
				for(File f:file.listFiles()){
					delete(f);
//					System.out.println("已删除"+f.getAbsolutePath());
				}
				file.delete();
//				System.out.println("已删除"+file.getAbsolutePath());
			}else{
				file.delete();
//				System.out.println("已删除"+file.getAbsolutePath());
			}
		}
	}
	
	public static void copyDir(String oldPath, String newPath) throws IOException {
        File file = new File(oldPath);
        String[] filePath = file.list();
        
        if (!(new File(newPath)).exists()) {
            (new File(newPath)).mkdir();
        }
        
        for (int i = 0; i < filePath.length; i++) {
            if ((new File(oldPath + File.separator + filePath[i])).isDirectory()) {
                copyDir(oldPath  + File.separator  + filePath[i], newPath  + File.separator + filePath[i]);
            }
            
            if (new File(oldPath  + File.separator + filePath[i]).isFile()) {
                copyFile(oldPath + File.separator + filePath[i], newPath + File.separator + filePath[i]);
            }
            
        }
    }
	
	public static void copyFile(String source, String dest) throws IOException {
		FileInputStream is = null;
	    FileOutputStream os = null;
		try {
			is= new FileInputStream(source);
			os = new FileOutputStream(dest);
	        byte[] buffer = new byte[1024];
	        int length;
	        while ((length = is.read(buffer)) > 0) {
	            os.write(buffer, 0, length);
	        }
	    } finally {
	        is.close();
	        os.close();
	    }
        
    }

	
	public static void ListFiles(String path,List<File> list) {
        File dir = new File(path);
        if (dir.exists()) {

            if (dir.isDirectory()) {
                File[] childs = dir.listFiles();
                for (File f : childs) {
                    ListFiles(f.getAbsolutePath(),null);
                }
            }
            System.out.println("ListFiles----" + dir.getAbsolutePath()+" isDirectory ? "+dir.isDirectory());
        }
    }
	
	
	  /**
     * 读取文件内容到数组
     *
     * @param filePath
     * @return
     */
    public static byte[] getFileByte(String filePath) {
        byte[] retBuffer = null;
        FileInputStream fis = null;
        ByteArrayOutputStream bos = null;
        try {
            File file = new File(filePath);
            fis = new FileInputStream(file);
            bos = new ByteArrayOutputStream(8192);
            byte[] b = new byte[8192];
            int n;
            while ((n = fis.read(b)) != -1) {
                bos.write(b, 0, n);
            }
            bos.flush();
            retBuffer = bos.toByteArray();
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            if (fis != null) {
                try {
                    fis.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
            if (bos != null) {
                try {
                    bos.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }

        }
        return retBuffer;
    }
    
    
    /**
     * 根据byte数组，生成文件
     *
     * @param bfile
     * @param filePath
     * @param fileName
     */
    public static void byteToFile(byte[] bfile, String filePath, String fileName) {
        BufferedOutputStream bos = null;
        FileOutputStream fos = null;
        File file = null;
        try {
            File dir = new File(filePath);
            if (!dir.exists() && dir.isDirectory()) {//判断文件目录是否存在
                System.out.println(filePath + "目录不存在");
                dir.mkdirs();
            }
            file = new File(filePath + File.separator + fileName);
            //System.out.println(file.getAbsolutePath());
            fos = new FileOutputStream(file);
            bos = new BufferedOutputStream(fos);
            bos.write(bfile);
            bos.flush();
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            if (bos != null) {
                try {
                    bos.close();
                } catch (IOException e1) {
                    e1.printStackTrace();
                }
            }
            if (fos != null) {
                try {
                    fos.close();
                } catch (IOException e1) {
                    e1.printStackTrace();
                }
            }
        }
    }
}
