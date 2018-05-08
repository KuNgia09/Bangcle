package com.fengyue.bangcle;

import java.util.Arrays;

import javax.crypto.Cipher;
import javax.crypto.spec.IvParameterSpec;
import javax.crypto.spec.SecretKeySpec;

import sun.misc.BASE64Decoder;
import sun.misc.BASE64Encoder;

public class AESUtil {

	//初始向量
	public static final String VIPARA = "1234567812345678";   //AES 为16bytes. DES 为8bytes
	//编码方式
	public static final String charset = "UTF-8";
	//私钥
	private static final String ASE_KEY="1234567812345678";   //AES固定格式为128/192/256 bits.即：16/24/32bytes。DES固定格式为128bits，即8bytes。
	
	private static String transformation = "AES/CBC/PKCS5Padding"; 
	
	private static String algorithm = "AES";

	
	public static byte[] encrypt(String content, String key) {  
        try {  
            SecretKeySpec skey = new SecretKeySpec(key.getBytes(), algorithm);  
            IvParameterSpec iv = new IvParameterSpec(key.getBytes());  
            Cipher cipher = Cipher.getInstance(transformation);  
            byte[] byteContent = content.getBytes(charset);  
            
            cipher.init(Cipher.ENCRYPT_MODE, skey, iv);// 初始化  
            byte[] result = cipher.doFinal(byteContent);  
            return result; // 加密  
        } catch (Exception e) {  
            e.printStackTrace();  
        }  
        return null;  
    }  
	
	public static byte[] encrypt(byte[] content, String key) {  
        try {  
            SecretKeySpec skey = new SecretKeySpec(key.getBytes(), algorithm);  
            IvParameterSpec iv = new IvParameterSpec(key.getBytes());  
            Cipher cipher = Cipher.getInstance(transformation);  
            
            cipher.init(Cipher.ENCRYPT_MODE, skey, iv);// 初始化  
            byte[] result = cipher.doFinal(content);  
            return result; // 加密  
        } catch (Exception e) {  
            e.printStackTrace();  
        }  
        return null;  
    }  
	
	

	/**
	 * 解密
	 * 
	 * @param encrypted
	 * @return
	 */
	public static String decrypt(String encryptData) {
		try {
            byte[] tmp=new BASE64Decoder().decodeBuffer(encryptData);
            byte[]  iv=new byte[16];
            Arrays.fill(iv, (byte)0);
			//IvParameterSpec zeroIv = new IvParameterSpec(iv);
            IvParameterSpec zeroIv = new IvParameterSpec(VIPARA.getBytes());
			SecretKeySpec key = new SecretKeySpec(
					ASE_KEY.getBytes(), "AES");
			Cipher cipher = Cipher.getInstance("AES/CBC/NoPadding");
			//与加密时不同MODE:Cipher.DECRYPT_MODE
			cipher.init(Cipher.DECRYPT_MODE, key, zeroIv);
			byte[] decryptedData = cipher.doFinal(tmp);
			System.out.println("解密后 hex data:"+byte2hex(decryptedData));
			System.out.println("加密后 hex data len:"+decryptedData.length);
			return new String(decryptedData, charset); 
		} catch (Exception e) {
			e.printStackTrace();
			return "";
		}
	}
	
	/**
     * 字节数组转成16进制字符串
     * @param b
     * @return
     */
    public static String byte2hex(byte[] b) { // 一个字节的数，
        StringBuffer sb = new StringBuffer(b.length * 2);
        String tmp = "";
        for (int n = 0; n < b.length; n++) {
            // 整数转成十六进制表示
            tmp = (Integer.toHexString(b[n] & 0XFF));
            if (tmp.length() == 1) {
                sb.append("0");
            }
            sb.append(tmp);
        }
        return sb.toString().toUpperCase(); // 转成大写
    }
    
    /**
     * 将hex字符串转换成字节数组
     * @param inputString
     * @return
     */
    private static byte[] hex2byte(String inputString) {
        if (inputString == null || inputString.length() < 2) {
            return new byte[0];
        }
        inputString = inputString.toLowerCase();
        int l = inputString.length() / 2;
        byte[] result = new byte[l];
        for (int i = 0; i < l; ++i) {
            String tmp = inputString.substring(2 * i, 2 * i + 2);
            result[i] = (byte) (Integer.parseInt(tmp, 16) & 0xFF);
        }
        return result;
    }

	
}