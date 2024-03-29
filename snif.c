#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netfilter_ipv4.h>
#include <linux/netfilter.h>
#include <linux/ip.h>

MODULE_LICENSE("GPL");

struct nf_hook_ops inp_ops;
struct nf_hook_ops out_ops;
int input = 1;
int output = 0;

unsigned int hook_func(void *priv,
					struct sk_buff *skb,
					const struct nf_hook_state *state)
{
	const char * direction_type;
	if (priv == NULL) 
		direction_type = "NULL direction??";
	else if (*(int*)priv == input) 
		direction_type = "input ";
	else if (*(int*)priv == output) 
		direction_type = "output";
	else direction_type = "unknown direction??";

	const char * packet_type;
	const char * header_type;

	unsigned char from[4];
	unsigned char to[4];
	*(uint*)from = 0;
	*(uint*)to = 0;
	int protocol_num = -1;
	if (skb->protocol == htons(ETH_P_IP)){
		packet_type = "ip";
		struct iphdr *ip = (struct iphdr *)skb_network_header(skb);
		switch (ip->protocol) {
			case IPPROTO_ICMP	: header_type = "ICMP";		break;
			case IPPROTO_IGMP	: header_type = "IGMP";		break;
			case IPPROTO_IPIP	: header_type = "IPIP";		break;
			case IPPROTO_TCP	: header_type = "TCP";		break;
			case IPPROTO_EGP	: header_type = "EGP";		break;
			case IPPROTO_PUP	: header_type = "PUP";		break;
			case IPPROTO_UDP	: header_type = "UDP";		break;
			case IPPROTO_IDP	: header_type = "IDP";		break;
			case IPPROTO_TP		: header_type = "TP";		break;
			case IPPROTO_DCCP	: header_type = "DCCP";		break;
			case IPPROTO_IPV6	: header_type = "IPV6";		break;
			case IPPROTO_RSVP	: header_type = "RSVP";		break;
			case IPPROTO_GRE	: header_type = "GRE";		break;
			case IPPROTO_ESP	: header_type = "ESP";		break;
			case IPPROTO_AH		: header_type = "AH";		break;
			case IPPROTO_MTP	: header_type = "MTP";		break;
			case IPPROTO_BEETPH	: header_type = "BEETPH";	break;
			case IPPROTO_ENCAP	: header_type = "ENCAP";	break;
			case IPPROTO_PIM	: header_type = "PIM";		break;
			case IPPROTO_COMP	: header_type = "COMP";		break;
			case IPPROTO_SCTP	: header_type = "SCTP";		break;
			case IPPROTO_UDPLITE: header_type = "UDPLITE";	break;
			case IPPROTO_MPLS	: header_type = "MPLS";		break;
			case IPPROTO_RAW	: header_type = "RAW";		break;
			case 200			: header_type = "NC";		break;
			default				: header_type = "unknown";
		}
		protocol_num = ip->protocol;
		*(uint*)from = ip->saddr;
		*(uint*)to = ip->daddr;
	}
	else packet_type = "unknown";
	
	printk("Snif: %s %s %s(%d) packet, from %u.%u.%u.%u to %u.%u.%u.%u\n", direction_type, packet_type, header_type, protocol_num, from[0], from[1], from[2], from[3], to[0], to[1], to[2], to[3]);
	return NF_ACCEPT;
}

int init_func(void){
	printk(KERN_INFO "Snif: Init!\n");
	inp_ops.hook = hook_func;
	inp_ops.priv = &input;
	inp_ops.pf = PF_INET;
	inp_ops.hooknum = NF_INET_PRE_ROUTING;
	inp_ops.priority = NF_IP_PRI_LAST;
	out_ops = inp_ops;
	out_ops.priv = &output;
	out_ops.hooknum = NF_INET_POST_ROUTING;
	nf_register_net_hook(&init_net, &inp_ops); 
	nf_register_net_hook(&init_net, &out_ops); 
	return 0;
}

void exit_func(void){
	nf_unregister_net_hook(&init_net, &out_ops);
	nf_unregister_net_hook(&init_net, &inp_ops);
	printk(KERN_INFO "Snif: Exit!\n");
}

module_init(init_func);
module_exit(exit_func);