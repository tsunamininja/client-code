/*
 * dns-packet.h
 *
 *  Created on: Jan 17, 2016
 *      Author: joe
 */

#ifndef DNS_PACKET_H_
#define DNS_PACKET_H_

// define the DNS packet structure

// define default values for DNS header fields

// set by client and copied in response from server

/* DNS HEADER FIELD & VALUES */
#define TRANSAC_ID		0xaabb  // should be randomized
#define QR_QUERY		0 		// message is query			// start flags
#define OPCODE			0 		// standard query
#define AA_NULL		    0       // *set by server
#define TC_NULL	        0		// *set by server
#define RD_TRUE			1		// recursion is desired
#define RA_NULL			0       // *set by server
#define Z				0		// reserved 3 bits for something
#define RCODE_NULL		0		// *set by server			// end flags
#define QDCOUNT			1		// we have 1 question
#define ANCOUNT_NULL	0		// *set by server
#define NSCOUNT_NULL	0		// *set by server
#define ARCOUNT_NULL	0		// *set by server

// shortcut flags
#define QUERYFLAGS      0x0100 // standard query

/* DNS QUESTION FIELD & VALUES */
#define QTYPE_A			1		// this is a query type for A records
#define QCLASS_IN		1		// this is a query of type Internet Class


/*
 * structure name - DNS_HEADER
 *
 * notes - the dns header follows the same format for both requests(queries)
 *         and responses.
 *
 * field name	purpose
 * ----------------------------------------------------------------------------
 * id			Identifies the query to the resolver and allows the response
 * 				to be associated with the initial query.
 *
 * qr			Specifies if message is a query (0) or a response (1).
 *
 * opcode		Identifies the kind of query in the message, 0 represents a
 * 				"standard" query.
 *
 * aa			Specifies whether or not the response received is from a name
 * 				server that	is an authority for the domain specified in query
 * 				message. If this bit is set, then the response is an
 * 				"Authoritative Answer".
 *
 * tc			Specifies that this message was truncated.
 *
 * rd			This bit directs the name server to pursue the query
 * 				recursively, client resolver software will usually set this bit.
 *
 * ra			This bit is set or cleared in a response and denotes whether
 * 				or not query support is available on the name server.
 *
 * z			Reserved for future use.
 *
 * rcode		This field is set as part of a response from a name server.
 * 				~~~~~~~~~~~~~~~~~~~~~~~
 * 				rcode number	meaning
 * 				0				no error condition
 *
 * 				1 				format error - server unable to interpret query
 *
 * 				2				server failure - unable to process query
 *
 * 				3				name error - meaningful only for responeses from
 * 								an authoritative name server, this value means
 * 								that the domain name referenced in the query
 * 								does not exist.
 *
 * 				4				not implemented - the name server does not
 * 								support the requested kind of query.
 *
 * 				5				refused - the name server refuses to perform
 * 								the specified operation for policy reasons.
 *
 * qdcount		This number specifies the number of entries in the question
 * 				section. If only sending one question, set field to 1.
 *
 * ancount		This number specfies the number of resource records in the
 * 				answer section. Clients set this field to 0 as there should
 * 				not be any resource records in a message of type query.
 *
 * nscount		This number specifies the number of name server resource records
 * 				in the authority records section.
 *
 * arcount		This number specifies the number of resource records in the
 * 				additional records section.
 */
struct DNS_HEADER
{
	unsigned short		id;
	unsigned short	 	flags;
	unsigned short		qdcode;
	unsigned short 	 	ancount;
	unsigned short		nscount;
	unsigned short		arcount;
};


/*
 * structure name - DNS_QUESTION
 *
 * notes -      The dns question follows the same format for both requests
 * 				(queries) and responses. It should be identical in the query
 * 				and in the response.
 *
 * field name	purpose
 * ----------------------------------------------------------------------------
 * qname		Represents a FQDN character string of ASCII values. This FQDN
 * 				is represented as a sequence of labels, where each label
 * 				consists of a length octet followed by that many number of
 * 				octets. The domain name terminates with the zero length octet
 * 				representing the null label of the root.
 *
 * qtype		Specifies this type of query. See chart below..
 * 				~~~~~~~~~~~~~~~~~~~~~~~
 * 				qtype value		meaning
 * 				1				A record query
 * 				16				A txt record query
 *
 * qclass		Specifies the class of the query. See chart below..
 *				qclass value	meaning
 * 				1				Internet Address *
 */
struct DNS_QUESTION
{
	unsigned char *qname;
	unsigned short qtype;
	unsigned short qclass;
};

/*
 * structure name - DNS_ANSWER
 *
 * notes - The dns answer section is only seen in responses from a dns server.
 * 		   Since the rdata field size is subject to the type field, make this
 * 		    a union thing? TBD 1/17/16.
 *
 *
 * field name	purpose
 * ----------------------------------------------------------------------------
 * name		    The domain name that was queried originally. The value of this
 * 				field is typically a pointer offset to the name in the DNS
 * 				Question Section / qname field.
 *
 * type			Specifies the meaning of the data in the rdata field of this
 * 				section.
 * 				~~~~~~~~~~~~~~~~~~~~~~~
 * 				type value		meaning
 * 				1				A record
 * 				5				CNAME
 *
 * class		Specifies the class of the data in the rdata field.
 *				class value		meaning
 * 				1				Internet Address *
 *
 * ttl			Specifies the number of seconds the results can be cached.
 *
 * rdlength		Specifies the length of the rdata field.
 *
 * rdata		Contains the data of the response. IE, type = A record then
 * 				returns	an IPv4 address (4 octets) in the rdata field.
 */
struct DNS_ANSWER
{
	unsigned short name;
	unsigned short type;
	unsigned short class;
	unsigned short ttl;
	unsigned short rdlength;
	unsigned int   rdata; // assuming a record queries
};

struct FQDN_NODE
{
	unsigned char *fqdn;
	unsigned int size;
	struct FQDN_NODE *next;
};

#endif /* DNS_PACKET_H_ */
