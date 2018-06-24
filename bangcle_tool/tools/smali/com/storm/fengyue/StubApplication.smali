.class public Lcom/storm/fengyue/StubApplication;
.super Landroid/app/Application;
.source "StubApplication.java"


# static fields
.field public static TAG:Ljava/lang/String;

.field public static soName:Ljava/lang/String;


# direct methods
.method static constructor <clinit>()V
    .locals 1

    .prologue
    .line 16
    const-string v0, "fengyue"

    sput-object v0, Lcom/storm/fengyue/StubApplication;->TAG:Ljava/lang/String;

    .line 17
    const-string v0, "libdexload"

    sput-object v0, Lcom/storm/fengyue/StubApplication;->soName:Ljava/lang/String;

    return-void
.end method

.method public constructor <init>()V
    .locals 0

    .prologue
    .line 15
    invoke-direct {p0}, Landroid/app/Application;-><init>()V

    return-void
.end method

.method public static copy(Landroid/content/Context;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z
    .locals 17
    .param p0, "context"    # Landroid/content/Context;
    .param p1, "asset_soname"    # Ljava/lang/String;
    .param p2, "str2"    # Ljava/lang/String;
    .param p3, "str3"    # Ljava/lang/String;

    .prologue
    .line 46
    new-instance v14, Ljava/lang/StringBuilder;

    invoke-static/range {p2 .. p2}, Ljava/lang/String;->valueOf(Ljava/lang/Object;)Ljava/lang/String;

    move-result-object v15

    invoke-direct {v14, v15}, Ljava/lang/StringBuilder;-><init>(Ljava/lang/String;)V

    sget-object v15, Ljava/io/File;->separator:Ljava/lang/String;

    invoke-virtual {v14, v15}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v14

    move-object/from16 v0, p3

    invoke-virtual {v14, v0}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v14

    invoke-virtual {v14}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v13

    .line 47
    .local v13, "so_dest":Ljava/lang/String;
    new-instance v6, Ljava/io/File;

    move-object/from16 v0, p2

    invoke-direct {v6, v0}, Ljava/io/File;-><init>(Ljava/lang/String;)V

    .line 48
    .local v6, "file":Ljava/io/File;
    invoke-virtual {v6}, Ljava/io/File;->exists()Z

    move-result v14

    if-nez v14, :cond_0

    .line 49
    invoke-virtual {v6}, Ljava/io/File;->mkdir()Z

    .line 51
    :cond_0
    new-instance v6, Ljava/io/File;

    .end local v6    # "file":Ljava/io/File;
    invoke-direct {v6, v13}, Ljava/io/File;-><init>(Ljava/lang/String;)V

    .line 53
    .restart local v6    # "file":Ljava/io/File;
    :try_start_0
    invoke-virtual {v6}, Ljava/io/File;->exists()Z

    move-result v14

    if-eqz v14, :cond_2

    .line 55
    invoke-virtual/range {p0 .. p0}, Landroid/content/Context;->getResources()Landroid/content/res/Resources;

    move-result-object v14

    invoke-virtual {v14}, Landroid/content/res/Resources;->getAssets()Landroid/content/res/AssetManager;

    move-result-object v14

    move-object/from16 v0, p1

    invoke-virtual {v14, v0}, Landroid/content/res/AssetManager;->open(Ljava/lang/String;)Ljava/io/InputStream;

    move-result-object v9

    .line 56
    .local v9, "open":Ljava/io/InputStream;
    new-instance v7, Ljava/io/FileInputStream;

    invoke-direct {v7, v6}, Ljava/io/FileInputStream;-><init>(Ljava/io/File;)V

    .line 57
    .local v7, "fileInputStream":Ljava/io/InputStream;
    new-instance v2, Ljava/io/BufferedInputStream;

    invoke-direct {v2, v9}, Ljava/io/BufferedInputStream;-><init>(Ljava/io/InputStream;)V

    .line 58
    .local v2, "bufferedInputStream":Ljava/io/BufferedInputStream;
    new-instance v3, Ljava/io/BufferedInputStream;

    invoke-direct {v3, v7}, Ljava/io/BufferedInputStream;-><init>(Ljava/io/InputStream;)V

    .line 59
    .local v3, "bufferedInputStream2":Ljava/io/BufferedInputStream;
    sget-object v14, Lcom/storm/fengyue/StubApplication;->TAG:Ljava/lang/String;

    const-string v15, "check is same file"

    invoke-static {v14, v15}, Landroid/util/Log;->d(Ljava/lang/String;Ljava/lang/String;)I

    .line 60
    invoke-static {v2, v3}, Lcom/storm/fengyue/StubApplication;->isSameFile(Ljava/io/BufferedInputStream;Ljava/io/BufferedInputStream;)Z

    move-result v14

    if-eqz v14, :cond_1

    .line 61
    const/4 v12, 0x1

    .line 65
    .local v12, "result":Z
    :goto_0
    invoke-virtual {v9}, Ljava/io/InputStream;->close()V

    .line 66
    invoke-virtual {v7}, Ljava/io/InputStream;->close()V

    .line 67
    invoke-virtual {v2}, Ljava/io/BufferedInputStream;->close()V

    .line 68
    invoke-virtual {v3}, Ljava/io/BufferedInputStream;->close()V

    .line 69
    if-eqz v12, :cond_2

    .line 95
    .end local v2    # "bufferedInputStream":Ljava/io/BufferedInputStream;
    .end local v3    # "bufferedInputStream2":Ljava/io/BufferedInputStream;
    .end local v7    # "fileInputStream":Ljava/io/InputStream;
    .end local v9    # "open":Ljava/io/InputStream;
    .end local v12    # "result":Z
    :goto_1
    return v12

    .line 63
    .restart local v2    # "bufferedInputStream":Ljava/io/BufferedInputStream;
    .restart local v3    # "bufferedInputStream2":Ljava/io/BufferedInputStream;
    .restart local v7    # "fileInputStream":Ljava/io/InputStream;
    .restart local v9    # "open":Ljava/io/InputStream;
    :cond_1
    const/4 v12, 0x0

    .restart local v12    # "result":Z
    goto :goto_0

    .line 73
    .end local v2    # "bufferedInputStream":Ljava/io/BufferedInputStream;
    .end local v3    # "bufferedInputStream2":Ljava/io/BufferedInputStream;
    .end local v7    # "fileInputStream":Ljava/io/InputStream;
    .end local v9    # "open":Ljava/io/InputStream;
    .end local v12    # "result":Z
    :cond_2
    invoke-virtual/range {p0 .. p0}, Landroid/content/Context;->getResources()Landroid/content/res/Resources;

    move-result-object v14

    invoke-virtual {v14}, Landroid/content/res/Resources;->getAssets()Landroid/content/res/AssetManager;

    move-result-object v14

    move-object/from16 v0, p1

    invoke-virtual {v14, v0}, Landroid/content/res/AssetManager;->open(Ljava/lang/String;)Ljava/io/InputStream;

    move-result-object v10

    .line 74
    .local v10, "open2":Ljava/io/InputStream;
    new-instance v8, Ljava/io/FileOutputStream;

    invoke-direct {v8, v13}, Ljava/io/FileOutputStream;-><init>(Ljava/lang/String;)V

    .line 75
    .local v8, "fileOutputStream":Ljava/io/FileOutputStream;
    const/16 v14, 0x1c00

    new-array v1, v14, [B

    .line 77
    .local v1, "bArr":[B
    :goto_2
    invoke-virtual {v10, v1}, Ljava/io/InputStream;->read([B)I

    move-result v11

    .line 78
    .local v11, "read":I
    if-gtz v11, :cond_3

    .line 83
    invoke-virtual {v8}, Ljava/io/FileOutputStream;->close()V

    .line 84
    invoke-virtual {v10}, Ljava/io/InputStream;->close()V
    :try_end_0
    .catch Ljava/io/FileNotFoundException; {:try_start_0 .. :try_end_0} :catch_0
    .catch Ljava/io/IOException; {:try_start_0 .. :try_end_0} :catch_1

    .line 86
    :try_start_1
    invoke-static {}, Ljava/lang/Runtime;->getRuntime()Ljava/lang/Runtime;

    move-result-object v14

    new-instance v15, Ljava/lang/StringBuilder;

    const-string v16, "chmod 755 "

    invoke-direct/range {v15 .. v16}, Ljava/lang/StringBuilder;-><init>(Ljava/lang/String;)V

    invoke-virtual {v15, v13}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v15

    invoke-virtual {v15}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v15

    invoke-virtual {v14, v15}, Ljava/lang/Runtime;->exec(Ljava/lang/String;)Ljava/lang/Process;
    :try_end_1
    .catch Ljava/lang/Exception; {:try_start_1 .. :try_end_1} :catch_2
    .catch Ljava/io/FileNotFoundException; {:try_start_1 .. :try_end_1} :catch_0
    .catch Ljava/io/IOException; {:try_start_1 .. :try_end_1} :catch_1

    .line 89
    :goto_3
    const/4 v12, 0x1

    goto :goto_1

    .line 81
    :cond_3
    const/4 v14, 0x0

    :try_start_2
    invoke-virtual {v8, v1, v14, v11}, Ljava/io/FileOutputStream;->write([BII)V
    :try_end_2
    .catch Ljava/io/FileNotFoundException; {:try_start_2 .. :try_end_2} :catch_0
    .catch Ljava/io/IOException; {:try_start_2 .. :try_end_2} :catch_1

    goto :goto_2

    .line 90
    .end local v1    # "bArr":[B
    .end local v8    # "fileOutputStream":Ljava/io/FileOutputStream;
    .end local v10    # "open2":Ljava/io/InputStream;
    .end local v11    # "read":I
    :catch_0
    move-exception v4

    .line 91
    .local v4, "e2":Ljava/io/FileNotFoundException;
    invoke-virtual {v4}, Ljava/io/FileNotFoundException;->printStackTrace()V

    .line 95
    .end local v4    # "e2":Ljava/io/FileNotFoundException;
    :goto_4
    const/4 v12, 0x0

    goto :goto_1

    .line 92
    :catch_1
    move-exception v5

    .line 93
    .local v5, "e3":Ljava/io/IOException;
    invoke-virtual {v5}, Ljava/io/IOException;->printStackTrace()V

    goto :goto_4

    .line 87
    .end local v5    # "e3":Ljava/io/IOException;
    .restart local v1    # "bArr":[B
    .restart local v8    # "fileOutputStream":Ljava/io/FileOutputStream;
    .restart local v10    # "open2":Ljava/io/InputStream;
    .restart local v11    # "read":I
    :catch_2
    move-exception v14

    goto :goto_3
.end method

.method public static isSameFile(Ljava/io/BufferedInputStream;Ljava/io/BufferedInputStream;)Z
    .locals 9
    .param p0, "bufferedInputStream"    # Ljava/io/BufferedInputStream;
    .param p1, "bufferedInputStream2"    # Ljava/io/BufferedInputStream;

    .prologue
    const/4 v6, 0x0

    .line 21
    :try_start_0
    invoke-virtual {p0}, Ljava/io/BufferedInputStream;->available()I

    move-result v0

    .line 22
    .local v0, "available":I
    invoke-virtual {p1}, Ljava/io/BufferedInputStream;->available()I

    move-result v1

    .line 23
    .local v1, "available2":I
    if-eq v0, v1, :cond_1

    .line 41
    .end local v0    # "available":I
    .end local v1    # "available2":I
    :cond_0
    :goto_0
    return v6

    .line 26
    .restart local v0    # "available":I
    .restart local v1    # "available2":I
    :cond_1
    new-array v2, v0, [B

    .line 27
    .local v2, "bArr":[B
    new-array v3, v1, [B

    .line 28
    .local v3, "bArr2":[B
    invoke-virtual {p0, v2}, Ljava/io/BufferedInputStream;->read([B)I

    .line 29
    invoke-virtual {p1, v3}, Ljava/io/BufferedInputStream;->read([B)I

    .line 30
    const/4 v1, 0x0

    :goto_1
    if-lt v1, v0, :cond_2

    .line 35
    const/4 v6, 0x1

    goto :goto_0

    .line 31
    :cond_2
    aget-byte v7, v2, v1

    aget-byte v8, v3, v1
    :try_end_0
    .catch Ljava/io/FileNotFoundException; {:try_start_0 .. :try_end_0} :catch_0
    .catch Ljava/io/IOException; {:try_start_0 .. :try_end_0} :catch_1

    if-ne v7, v8, :cond_0

    .line 30
    add-int/lit8 v1, v1, 0x1

    goto :goto_1

    .line 36
    .end local v0    # "available":I
    .end local v1    # "available2":I
    .end local v2    # "bArr":[B
    .end local v3    # "bArr2":[B
    :catch_0
    move-exception v4

    .line 37
    .local v4, "e":Ljava/io/FileNotFoundException;
    invoke-virtual {v4}, Ljava/io/FileNotFoundException;->printStackTrace()V

    goto :goto_0

    .line 39
    .end local v4    # "e":Ljava/io/FileNotFoundException;
    :catch_1
    move-exception v5

    .line 40
    .local v5, "e2":Ljava/io/IOException;
    invoke-virtual {v5}, Ljava/io/IOException;->printStackTrace()V

    goto :goto_0
.end method


# virtual methods
.method protected attachBaseContext(Landroid/content/Context;)V
    .locals 5
    .param p1, "context"    # Landroid/content/Context;

    .prologue
    .line 99
    invoke-super {p0, p1}, Landroid/app/Application;->attachBaseContext(Landroid/content/Context;)V

    .line 100
    sget-object v2, Lcom/storm/fengyue/StubApplication;->TAG:Ljava/lang/String;

    const-string v3, "StubApplication.attachBaseContext"

    invoke-static {v2, v3}, Landroid/util/Log;->d(Ljava/lang/String;Ljava/lang/String;)I

    .line 101
    new-instance v2, Ljava/lang/StringBuilder;

    invoke-virtual {p1}, Landroid/content/Context;->getFilesDir()Ljava/io/File;

    move-result-object v3

    invoke-virtual {v3}, Ljava/io/File;->getAbsolutePath()Ljava/lang/String;

    move-result-object v3

    invoke-static {v3}, Ljava/lang/String;->valueOf(Ljava/lang/Object;)Ljava/lang/String;

    move-result-object v3

    invoke-direct {v2, v3}, Ljava/lang/StringBuilder;-><init>(Ljava/lang/String;)V

    const-string v3, "/.jiagu"

    invoke-virtual {v2, v3}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v2

    invoke-virtual {v2}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v1

    .line 102
    .local v1, "str":Ljava/lang/String;
    sget-object v0, Landroid/os/Build;->CPU_ABI:Ljava/lang/String;

    .line 103
    .local v0, "abi":Ljava/lang/String;
    sget-object v2, Lcom/storm/fengyue/StubApplication;->TAG:Ljava/lang/String;

    new-instance v3, Ljava/lang/StringBuilder;

    const-string v4, "Build.CPU_ABI:"

    invoke-direct {v3, v4}, Ljava/lang/StringBuilder;-><init>(Ljava/lang/String;)V

    invoke-virtual {v3, v0}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v3

    invoke-virtual {v3}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v3

    invoke-static {v2, v3}, Landroid/util/Log;->d(Ljava/lang/String;Ljava/lang/String;)I

    .line 105
    const-string v2, "armeabi"

    invoke-virtual {v0, v2}, Ljava/lang/String;->contains(Ljava/lang/CharSequence;)Z

    move-result v2

    if-eqz v2, :cond_0

    .line 106
    new-instance v2, Ljava/lang/StringBuilder;

    sget-object v3, Lcom/storm/fengyue/StubApplication;->soName:Ljava/lang/String;

    invoke-static {v3}, Ljava/lang/String;->valueOf(Ljava/lang/Object;)Ljava/lang/String;

    move-result-object v3

    invoke-direct {v2, v3}, Ljava/lang/StringBuilder;-><init>(Ljava/lang/String;)V

    const-string v3, "_arm.so"

    invoke-virtual {v2, v3}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v2

    invoke-virtual {v2}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v2

    new-instance v3, Ljava/lang/StringBuilder;

    sget-object v4, Lcom/storm/fengyue/StubApplication;->soName:Ljava/lang/String;

    invoke-static {v4}, Ljava/lang/String;->valueOf(Ljava/lang/Object;)Ljava/lang/String;

    move-result-object v4

    invoke-direct {v3, v4}, Ljava/lang/StringBuilder;-><init>(Ljava/lang/String;)V

    const-string v4, ".so"

    invoke-virtual {v3, v4}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v3

    invoke-virtual {v3}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v3

    invoke-static {p1, v2, v1, v3}, Lcom/storm/fengyue/StubApplication;->copy(Landroid/content/Context;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z

    .line 107
    new-instance v2, Ljava/lang/StringBuilder;

    invoke-static {v1}, Ljava/lang/String;->valueOf(Ljava/lang/Object;)Ljava/lang/String;

    move-result-object v3

    invoke-direct {v2, v3}, Ljava/lang/StringBuilder;-><init>(Ljava/lang/String;)V

    sget-object v3, Ljava/io/File;->separator:Ljava/lang/String;

    invoke-virtual {v2, v3}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v2

    sget-object v3, Lcom/storm/fengyue/StubApplication;->soName:Ljava/lang/String;

    invoke-virtual {v2, v3}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v2

    const-string v3, ".so"

    invoke-virtual {v2, v3}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v2

    invoke-virtual {v2}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v2

    invoke-static {v2}, Ljava/lang/System;->load(Ljava/lang/String;)V

    .line 108
    invoke-static {p1}, Lcom/storm/fengyue/Native;->attachBaseContext(Landroid/content/Context;)V

    .line 122
    :goto_0
    return-void

    .line 111
    :cond_0
    const-string v2, "arm64"

    invoke-virtual {v0, v2}, Ljava/lang/String;->contains(Ljava/lang/CharSequence;)Z

    move-result v2

    if-eqz v2, :cond_1

    .line 113
    new-instance v2, Ljava/lang/StringBuilder;

    sget-object v3, Lcom/storm/fengyue/StubApplication;->soName:Ljava/lang/String;

    invoke-static {v3}, Ljava/lang/String;->valueOf(Ljava/lang/Object;)Ljava/lang/String;

    move-result-object v3

    invoke-direct {v2, v3}, Ljava/lang/StringBuilder;-><init>(Ljava/lang/String;)V

    const-string v3, "_a64.so"

    invoke-virtual {v2, v3}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v2

    invoke-virtual {v2}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v2

    new-instance v3, Ljava/lang/StringBuilder;

    sget-object v4, Lcom/storm/fengyue/StubApplication;->soName:Ljava/lang/String;

    invoke-static {v4}, Ljava/lang/String;->valueOf(Ljava/lang/Object;)Ljava/lang/String;

    move-result-object v4

    invoke-direct {v3, v4}, Ljava/lang/StringBuilder;-><init>(Ljava/lang/String;)V

    const-string v4, ".so"

    invoke-virtual {v3, v4}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v3

    invoke-virtual {v3}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v3

    invoke-static {p1, v2, v1, v3}, Lcom/storm/fengyue/StubApplication;->copy(Landroid/content/Context;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z

    .line 114
    new-instance v2, Ljava/lang/StringBuilder;

    invoke-static {v1}, Ljava/lang/String;->valueOf(Ljava/lang/Object;)Ljava/lang/String;

    move-result-object v3

    invoke-direct {v2, v3}, Ljava/lang/StringBuilder;-><init>(Ljava/lang/String;)V

    sget-object v3, Ljava/io/File;->separator:Ljava/lang/String;

    invoke-virtual {v2, v3}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v2

    sget-object v3, Lcom/storm/fengyue/StubApplication;->soName:Ljava/lang/String;

    invoke-virtual {v2, v3}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v2

    const-string v3, ".so"

    invoke-virtual {v2, v3}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v2

    invoke-virtual {v2}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v2

    invoke-static {v2}, Ljava/lang/System;->load(Ljava/lang/String;)V

    .line 115
    invoke-static {p1}, Lcom/storm/fengyue/Native;->attachBaseContext(Landroid/content/Context;)V

    goto :goto_0

    .line 119
    :cond_1
    sget-object v2, Lcom/storm/fengyue/StubApplication;->TAG:Ljava/lang/String;

    new-instance v3, Ljava/lang/StringBuilder;

    const-string v4, "Bangcle is not supported abi:"

    invoke-direct {v3, v4}, Ljava/lang/StringBuilder;-><init>(Ljava/lang/String;)V

    invoke-virtual {v3, v0}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v3

    invoke-virtual {v3}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v3

    invoke-static {v2, v3}, Landroid/util/Log;->e(Ljava/lang/String;Ljava/lang/String;)I

    goto :goto_0
.end method

.method public onCreate()V
    .locals 2

    .prologue
    .line 125
    invoke-super {p0}, Landroid/app/Application;->onCreate()V

    .line 126
    sget-object v0, Lcom/storm/fengyue/StubApplication;->TAG:Ljava/lang/String;

    const-string v1, "StubApplication.onCreate"

    invoke-static {v0, v1}, Landroid/util/Log;->d(Ljava/lang/String;Ljava/lang/String;)I

    .line 127
    invoke-static {p0}, Lcom/storm/fengyue/Native;->onCreate(Landroid/content/Context;)V

    .line 128
    return-void
.end method
