#ifndef	__TSI_DOC_H
#define	__TSI_DOC_H 

#define COTULLA
#define DOC_PHYS_ADDR		0x14000000
#define	DOC_PHYS_SIZE		0x2000		

int 	tsi_request_doc_iomem	();
int 	tsi_release_doc_iomem	();
void *	tsi_get_doc_vaddr	();

#endif
