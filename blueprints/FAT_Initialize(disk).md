FAT_Initialize(disk)
│
├── FAT_ReadBootSector(disk)
│   └── DISK_ReadSectors(disk, 0, 1, BS.BootSectorBytes)
│       └── fills BS union (both BootSector fields + raw bytes)
│
├── FAT_ReadFat(disk)
│   └── DISK_ReadSectors(disk, ReservedSectors, SectorsPerFat, g_Fat)
│       └── fills g_Fat[] with cluster chain table
│
├── DISK_ReadSectors(disk, rootDirLba, 1, RootDirectory.Buffer)
│   └── fills RootDirectory.Buffer with first sector of root dir
│
└── sets g_DataSectionLba (start of data region)


FAT_Open(disk, path)
│
├── starts at &g_Data->RootDirectory.Public (FAT_File*)
│
├── FAT_FindFile(disk, currentDir, name, &entry)
│   │
│   └── FAT_ReadEntry(disk, file, &dirEntry)
│       │
│       └── FAT_Read(disk, file, sizeof(FAT_DirectoryEntry), dirEntry)
│           │
│           └── DISK_ReadSectors(disk, lba, 1, fd->Buffer)
│               └── returns bytes read
│               └── updates file->Position
│
└── FAT_OpenEntry(disk, &entry)
    │
    ├── finds free slot in OpenedFiles[]
    ├── fills FAT_FileData:
    │   ├── FirstCluster
    │   ├── CurrentCluster
    │   ├── CurrentSectorInCluster
    │   └── Public (Handle, IsDirectory, Position, Size)
    │
    └── returns FAT_File* (public handle to caller)


FAT_Read(disk, file, byteCount, dataOut)
│
├── uses file->Handle → finds FAT_FileData in OpenedFiles[]
├── reads from fd->Buffer (sector cache)
├── if sector exhausted:
│   ├── FAT_NextCluster(currentCluster)
│   │   └── reads g_Fat[] to find next cluster in chain
│   └── FAT_ClusterToLba(cluster)
│       └── uses g_DataSectionLba + (cluster - 2) * SectorsPerCluster
│
└── returns bytes actually read


FAT_ReadEntry(disk, file, dirEntry)
│
└── FAT_Read(disk, file, sizeof(FAT_DirectoryEntry), dirEntry)
    └── returns true if full entry was read


FAT_Close(file)
│
├── if ROOT_DIRECTORY_HANDLE
│   └── resets Position + CurrentCluster to FirstCluster
│
└── else
    └── sets OpenedFiles[handle].Opened = false


FAT_FindFile(disk, file, name, entryOut)
│
├── converts name to FAT 8.3 format
│   ├── base name (up to 8 chars, stops at '.')
│   └── extension (up to 3 chars after '.')
│
└── loops FAT_ReadEntry() until name matches
    └── returns FAT_DirectoryEntry* to caller


FAT_ListDirectory(disk, dir)
│
└── loops FAT_ReadEntry()
    └── prints each entry name found in directory
